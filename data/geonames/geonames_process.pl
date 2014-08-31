#! /usr/bin/perl -w

use strict;
use IO::File;
use XML::Writer;

my %time_zones = ();
my %countries = ();

open(TIMEZONES, 'timeZones.txt') or die("Cannot open timeZones.txt: $!\n");
while (<TIMEZONES>) {
    my ($country_code, $timezone_id, $gmt_offset_january, $gmt_offset_july, $gmt_offset_raw) = split(/\t/, $_);
    next if ($country_code !~ /^[A-Z]{2}$/);

    $time_zones{$timezone_id} = {offset => $gmt_offset_january, dst_offset => $gmt_offset_july};
}
close(TIMEZONES);

my $xml_file = IO::File->new('>geodata.xml');
my $writer = XML::Writer->new(OUTPUT => $xml_file, NEWLINES => 0);

$writer->xmlDecl('utf-8');
$writer->startTag('geodata');
$writer->startTag('countries');

open(COUNTRIES, 'countryInfo.txt') or die("Cannot open countryInfo.txt: $!\n");
while (<COUNTRIES>) {
    my ($country_code, $iso3, $iso_numeric, $fips, $name, $capital, $area, $population, $continent, $tld, $currency_code, $currency_name, $phone, $postal_code_format, $postal_code_regex, $languages, $geonameid, $neighbours, $equivalent_fips_code) = split(/\t/, $_);
    next if ($country_code !~ /^[A-Z]{2}$/);

    $writer->emptyTag('c',
            'n'   => $name,
            'c'   => $country_code,
        );

    if ($country_code =~ /^[A-Z]{2}$/) {
        $countries{$country_code} = $name;
    }
}
close(COUNTRIES);

$writer->endTag('countries');
$writer->startTag('places');

open(GEONAMES, "cities.txt") or die("Cannot open cities.txt: $!\n");

while (<GEONAMES>) {
    chomp($_);
    my ($country_code, $name, $latitude, $longitude, $elevation, $timezone) = split(/\t/, $_);

    if (!exists($countries{$country_code})) {
        print "Unknown country code: $country_code\n";
        next;
    }

    if (!exists($time_zones{$timezone})) {
        print "Unknown time zone: $timezone\n";
        next;
    }

    $writer->emptyTag('p',
            'n'   => $name,
            'lat' => $latitude,
            'lon' => $longitude,
            'alt' => $elevation,
            'c'   => $country_code,
            'tzo' => $time_zones{$timezone}->{offset},
            'tzd' => $time_zones{$timezone}->{dst_offset}
        );

    print $., "\n" if ($. % 19083 == 0);
}

$writer->endTag('places');
$writer->endTag('geodata');
$writer->end();
$xml_file->close();

close GEONAMES;

