#! /usr/bin/perl -w

use strict;
use IO::File;
use XML::Writer;

# TODO: Download http://download.geonames.org/export/dump/countryInfo.txt
# TODO: Download http://download.geonames.org/export/dump/timeZones.txt
# TODO: Download http://download.geonames.org/export/dump/allCountries.zip
# TODO: Unzip allCountries.zip

my %time_zones = ();
my %countries = ();

open(TIMEZONES, 'timeZones.txt');
while (<TIMEZONES>) {
    my ($country_code, $timezone_id, $gmt_offset_january, $gmt_offset_july, $gmt_offset_raw) = split(/\t/, $_);

    $time_zones{$country_code . '_' . $timezone_id} = {offset => $gmt_offset_january, dst_offset => $gmt_offset_july};
}
close(TIMEZONES);

open(COUNTRIES, 'countryInfo.txt');
while (<COUNTRIES>) {
    my ($country_code, $iso3, $iso_numeric, $fips, $name, $capital, $area, $population, $continent, $tld, $currency_code, $currency_name, $phone, $postal_code_format, $postal_code_regex, $languages, $geonameid, $neighbours, $equivalent_fips_code) = split(/\t/, $_);

    if ($country_code =~ /^[A-Z]{2}$/) {
        $countries{$country_code} = $name;
    }
}
close(COUNTRIES);

my $xml_file = IO::File->new('>geodata.xml');
my $writer = XML::Writer->new(OUTPUT => $xml_file, NEWLINES => 0);

$writer->xmlDecl('utf-8');
$writer->startTag('geodata');

# TODO: process all files, not just HU.txt!
open(GEONAMES, "HU.txt");
while (<GEONAMES>) {
    my ($geonameid, $name, $asciiname, $alternatenames, $latitude, $longitude, $feature_class, $feature_code, $country_code, $alt_country_code, $admin1, $admin2, $admin3, $admin4, $population, $elevation, $dem, $timezone, $mod_date) = split(/\t/, $_);

    if (($feature_class eq 'P') && ($feature_code eq 'PPL')) {
        $writer->emptyTag('place',
                'name'            => $name,
                'latitude'        => $latitude,
                'longitude'       => $longitude,
                'elevation'       => $elevation,
                'country'         => $countries{$country_code},
                'time_offset'     => $time_zones{$country_code . '_' . $timezone}->{offset},
                'time_offset_dst' => $time_zones{$country_code . '_' . $timezone}->{dst_offset}
            );
    }
}
close GEONAMES;

$writer->endTag('geodata');

