<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    xmlns:math="http://exslt.org/math">
    <xsl:output
        method="xml"
        media-type="image/svg+xml"
        doctype-public="-//W3C//DTD SVG 1.0//EN"
        doctype-system="http://www.w3.org/TR/2001/PR-SVG-20010719/DTD/svg10.dtd"
        version="1.0"
        encoding="UTF-8"
        indent="yes"/>
    <xsl:variable name="asc" select="chartinfo/ascmcs/ascendant/@degree_ut" />
    <xsl:variable name="asc_rotate" select="$asc - 180"/>
    <xsl:variable name="PI" select="math:constant('PI', 10)" />
    <xsl:variable name="r_aspect" select="240" />

    <xsl:template match="/">
        <xsl:processing-instruction name="xml-stylesheet">href="<xsl:value-of select="$css_file"/>" type="text/css"</xsl:processing-instruction>
        <svg
            xmlns="http://www.w3.org/2000/svg"
            xmlns:xlink="http://www.w3.org/1999/xlink"
            version="1.0"
            width="800"
            height="800">
            <title>
                <xsl:value-of select="concat(
                    'Natal chart of ',
                    chartinfo/data/name/text()
                    )"/>
            </title>
            <defs>
                <xi:include href="sign_aries.xml" />
                <xi:include href="sign_taurus.xml" />
                <xi:include href="sign_gemini.xml" />
                <xi:include href="sign_cancer.xml" />
                <xi:include href="sign_leo.xml" />
                <xi:include href="sign_virgo.xml" />
                <xi:include href="sign_libra.xml" />
                <xi:include href="sign_scorpio.xml" />
                <xi:include href="sign_sagittarius.xml" />
                <xi:include href="sign_capricorn.xml" />
                <xi:include href="sign_aquarius.xml" />
                <xi:include href="sign_pisces.xml" />

                <marker id="arrow_end" orient="auto" refX="-3.5" refY="0.0" style="overflow:visible">
                    <polygon points="0.0,0.0 7.0,-2.0 5.0,0.0 7.0,2.0" transform="rotate(180)" />
                </marker>
                <path
                    id="planet_sun_tmpl"
                    class="planet-symbol"
                    d="m 29.21875,14.60938 c -4e-5,4.04949 -1.42582,7.5 -4.27735,10.35156 -2.85158,2.83854 -6.3021,4.25781 -10.35156,4.25781 -4.02345,0 -7.46095,-1.41927 -10.3125,-4.25781 -2.85157,-2.85156 -4.27735,-6.30207 -4.27734,-10.35156 -10e-6,-2.52603 0.65103,-4.96092 1.95312,-7.30469 1.3151,-2.35675 3.14452,-4.16013 5.48828,-5.41016 2.34374,-1.26299 4.72655,-1.8945 7.14844,-1.89453 2.46092,3e-5 4.86326,0.63154 7.20703,1.89453 2.34372,1.25003 4.16664,3.0469 5.46875,5.39063 1.30205,2.33075 1.95309,4.77215 1.95313,7.32422 m -27.79297,0 c -10e-6,3.64584 1.28254,6.75131 3.84765,9.3164 2.5651,2.55209 5.67056,3.82813 9.31641,3.82813 3.64581,0 6.75128,-1.27604 9.31641,-3.82813 2.56507,-2.56509 3.84762,-5.67056 3.84765,-9.3164 -3e-5,-2.30467 -0.58597,-4.50519 -1.75781,-6.60157 -1.1719,-2.09633 -2.81253,-3.71091 -4.92187,-4.84375 -2.1094,-1.13278 -4.27086,-1.69919 -6.48438,-1.69922 -2.21356,3e-5 -4.37501,0.56644 -6.48437,1.69922 -2.10939,1.13284 -3.75001,2.74742 -4.92188,4.84375 -1.17188,2.09638 -1.75782,4.2969 -1.75781,6.60157 m 16.83594,0 c -3e-5,1.01564 -0.3581,1.88152 -1.07422,2.59765 -0.70315,0.70314 -1.56252,1.0547 -2.57813,1.05469 -1.00262,1e-5 -1.86199,-0.35155 -2.57812,-1.05469 -0.71616,-0.71613 -1.07424,-1.58201 -1.07422,-2.59765 -2e-5,-0.63801 0.16274,-1.24999 0.48828,-1.83594 0.32551,-0.58592 0.78123,-1.03514 1.36719,-1.34766 0.58592,-0.31248 1.18488,-0.46873 1.79687,-0.46875 0.61196,2e-5 1.21092,0.15627 1.79688,0.46875 0.58591,0.31252 1.04164,0.76174 1.36718,1.34766 0.3255,0.58595 0.48826,1.19793 0.48829,1.83594"/>
                <path
                    id="planet_moon_tmpl"
                    class="planet-symbol"
                    d="m 3.14453,29.21875 c 5.14324,0 9.49219,-2.45442 13.04687,-7.36328 1.36719,-2.25259 2.05079,-4.66796 2.05078,-7.24609 10e-6,-5.10415 -2.57812,-9.32941 -7.73437,-12.67578 -2.33072,-1.28904 -4.64842,-1.93357 -6.95313,-1.9336 l -0.95703,0 c -0.59894,3e-5 -1.41925,0.0977 -2.46094,0.29297 l 0,0.13672 c 3.59377,1.02867 6.21096,2.69534 7.85157,5 2.00522,2.64325 3.00782,5.70314 3.00781,9.17969 1e-5,5.20834 -2.17447,9.27735 -6.52344,12.20703 -1.77081,1.02864 -3.2617,1.64713 -4.47265,1.85547 l 0,0.0781 c 2e-5,0.1302 0.97007,0.28645 2.91015,0.46875 l 0.23438,0 m 0.54687,-1.21094 0,-0.0586 c 5.95053,-2.5651 8.92579,-7.05728 8.92578,-13.47656 10e-6,-5.22134 -2.18749,-9.34243 -6.5625,-12.36328 l -0.85937,-0.46875 0,-0.0586 0.11719,0 c 1.1198,3e-5 2.71485,0.54039 4.78515,1.6211 3.02084,1.95314 5.07162,4.52476 6.15235,7.71484 0.36458,1.19793 0.54688,2.40887 0.54687,3.63281 l 0,0.17578 c 10e-6,4.14064 -2.2526,7.90366 -6.75781,11.28907 -2.13541,1.32812 -4.23176,1.99218 -6.28906,1.99218 l -0.0586,0"/>
                <path
                    id="planet_mercury_tmpl"
                    class="planet-symbol"
                    d="m 13.63282,7.59765 c 0.52081,0.33857 1.0156,0.74221 1.48437,1.21094 1.73175,1.70575 2.59764,3.78908 2.59766,6.25 -2e-5,2.44793 -0.8594,4.47918 -2.57813,6.09375 -1.40627,1.30209 -3.04038,2.04428 -4.90234,2.22656 l 0,4.08203 4.21875,-0.0391 0,1.95313 -4.29688,0.0195 0.0195,3.94531 -2.51953,0 -0.0195,-3.96484 -3.94531,0 0,-1.91406 3.94531,0 -0.0391,-4.08204 c -1.91407,-0.16926 -3.58073,-0.91145 -5,-2.22656 -1.73177,-1.61457 -2.59766,-3.64582 -2.59765,-6.09375 -1e-5,-2.46092 0.86588,-4.54425 2.59765,-6.25 0.46875,-0.46873 0.96354,-0.87237 1.48438,-1.21093 -0.52084,-0.3255 -1.01563,-0.72915 -1.48438,-1.21094 -1.73177,-1.70571 -2.59766,-3.78253 -2.59765,-6.23047 l 0,-0.15625 2.77343,0 0,0.19531 c 0,1.69274 0.59245,3.12503 1.77735,4.29688 1.10676,1.08075 2.42186,1.66669 3.94531,1.75781 l 0.74219,0 c 1.5104,-0.0911 2.81899,-0.67706 3.92578,-1.75781 1.18488,-1.17185 1.77732,-2.60414 1.77734,-4.29688 l 0,-0.19531 2.77344,0 0,0.15625 c -2e-5,2.44794 -0.86591,4.52476 -2.59766,6.23047 -0.46876,0.48179 -0.96356,0.88544 -1.48437,1.21094 m -4.39453,1.36718 -0.74219,0 c -1.52345,0.0781 -2.83855,0.66408 -3.94531,1.75782 -1.1849,1.15887 -1.77735,2.58465 -1.77735,4.27734 0,1.69272 0.59245,3.09246 1.77735,4.19922 1.19791,1.10678 2.64322,1.66016 4.33593,1.66015 1.65364,1e-5 3.07291,-0.55337 4.25782,-1.66015 1.1979,-1.10676 1.79685,-2.5065 1.79687,-4.19922 -2e-5,-1.69269 -0.59246,-3.11847 -1.77734,-4.27734 -1.10679,-1.09374 -2.41538,-1.67967 -3.92578,-1.75782"/>
                <path
                    id="planet_venus_tmpl"
                    class="planet-symbol"
                    d="m 11.23047,3.49609 c -2.13543,3e-5 -3.95834,0.74222 -5.46875,2.22657 -1.4974,1.48439 -2.2461,3.29429 -2.24609,5.42968 -10e-6,2.12241 0.74869,3.88674 2.24609,5.29297 1.51041,1.40626 3.33332,2.10939 5.46875,2.10938 2.09634,10e-6 3.90623,-0.70312 5.42969,-2.10938 1.52341,-1.40623 2.28513,-3.17056 2.28516,-5.29297 -3e-5,-2.13539 -0.76175,-3.94529 -2.28516,-5.42968 -1.52346,-1.48435 -3.33335,-2.22654 -5.42969,-2.22657 m -7.92969,15.3711 c -2.20052,-2.04426 -3.30078,-4.61587 -3.30078,-7.71485 0,-3.11196 1.09375,-5.74867 3.28125,-7.91015 2.18749,-2.16143 4.83723,-3.24216 7.94922,-3.24219 3.0729,3e-5 5.70962,1.08076 7.91016,3.24219 2.21351,2.16148 3.32029,4.79819 3.32031,7.91015 -2e-5,3.112 -1.10028,5.68361 -3.30078,7.71485 -1.77085,1.64063 -3.82815,2.57813 -6.17188,2.8125 l 0,5.15625 5.3125,0 0,2.42187 -5.3125,0 0,5.01953 -3.51562,0 0,-5.01953 -5.3125,0 0,-2.42187 5.3125,0 0,-5.17579 c -2.35678,-0.22134 -4.41407,-1.15233 -6.17188,-2.79296"/>
                <path
                    id="planet_mars_tmpl"
                    class="planet-symbol"
                    d="m 11.23047,10.8789 c -2.13543,2e-5 -3.95835,0.74221 -5.46875,2.22657 -1.49741,1.48439 -2.2461,3.29428 -2.2461,5.42968 0,2.13543 0.74869,3.90626 2.2461,5.3125 1.5104,1.40626 3.33332,2.10938 5.46875,2.10938 2.09633,0 3.90623,-0.70312 5.42968,-2.10938 1.52342,-1.40624 2.28514,-3.17707 2.28516,-5.3125 -2e-5,-2.1354 -0.76174,-3.94529 -2.28516,-5.42968 -1.52345,-1.48436 -3.33335,-2.22655 -5.42968,-2.22657 m 8.04687,15.29297 c -2.18752,2.13542 -4.86981,3.20313 -8.04687,3.20313 -3.05991,0 -5.69663,-1.02865 -7.91016,-3.08594 -2.21355,-2.07031 -3.32032,-4.65494 -3.32031,-7.75391 -10e-6,-3.09894 1.11978,-5.75519 3.35937,-7.96875 2.2526,-2.22654 4.87629,-3.28122 7.8711,-3.16406 2.96873,0.10419 5.18227,0.82685 6.64062,2.16797 l 7.05078,-6.79687 -7.24609,0 2.73437,-2.77344 9.64844,0 0,9.60937 -2.79297,2.79297 0,-7.28515 -6.99219,6.79687 c 1.45831,1.90106 2.18748,4.10809 2.1875,6.62109 -2e-5,3.02084 -1.06122,5.56641 -3.18359,7.63672"/>
                <path
                    id="planet_jupiter_tmpl"
                    class="planet-symbol"
                    d="m 8.63281,18.84766 0,-0.0195 c 1.58853,10e-6 2.93618,-0.55338 4.04297,-1.66016 1.10675,-1.11978 1.66013,-2.46743 1.66016,-4.04297 -3e-5,-1.5755 -0.55341,-2.91665 -1.66016,-4.02344 -1.09377,-1.11977 -2.44142,-1.67966 -4.04297,-1.67968 -1.57553,2e-5 -2.92319,0.55991 -4.04297,1.67968 -1.10678,1.10679 -1.66016,2.44794 -1.66015,4.02344 l 0,0.0391 -2.92969,0 c -10e-6,-2.40883 0.83983,-4.45961 2.51953,-6.15234 1.67968,-1.69268 3.71743,-2.53904 6.11328,-2.53906 2.43488,2e-5 4.49217,0.84638 6.17188,2.53906 1.67966,1.69273 2.5195,3.74351 2.51953,6.15234 -3e-5,2.18752 -0.70966,4.08205 -2.12891,5.6836 l 4.43359,0 0,-18.84766 2.94922,0 0,29.21875 -2.94922,0 0,-7.42187 -19.6289,0 0,-2.94922 8.63281,0"/>
                <path
                    id="planet_saturn_tmpl"
                    class="planet-symbol"
                    d="m 2.07032,6.46485 -2.07032,0 0,-2.94922 2.07032,0 0,-3.51563 2.96875,0 0,3.51563 3.47656,0 0,2.94922 -3.47656,0 0,7.55859 c 1.44529,-1.41926 3.15753,-2.12889 5.13672,-2.12891 2.31768,2e-5 4.25127,0.83986 5.80078,2.51953 1.56247,1.6797 2.34372,3.73699 2.34375,6.17188 -3e-5,1.17188 -0.45576,2.50652 -1.36719,4.0039 -0.91148,1.48438 -1.36721,3.01433 -1.36719,4.58985 l 0,0.0391 -2.94922,0 c -2e-5,-1.71875 0.45571,-3.35286 1.36719,-4.90235 0.92446,-1.54947 1.38669,-2.79296 1.38672,-3.73046 -3e-5,-1.60156 -0.50784,-2.94921 -1.52344,-4.04297 -1.00262,-1.10676 -2.22658,-1.66014 -3.67187,-1.66016 -1.43231,2e-5 -2.64976,0.5534 -3.65235,1.66016 -1.00262,1.10678 -1.50392,2.45443 -1.5039,4.04297 l 0,8.63281 -2.96875,0 0,-22.75391"/>
                <path
                    id="planet_uranus_tmpl"
                    class="planet-symbol"
                    d="m 11.23047,26.26953 c -0.83335,10e-6 -1.56902,-0.29296 -2.20703,-0.8789 -0.62501,-0.59895 -0.93751,-1.34114 -0.9375,-2.22656 -10e-6,-0.80728 0.30598,-1.49739 0.91797,-2.07032 0.61196,-0.5729 1.35415,-0.85936 2.22656,-0.85937 0.91144,10e-6 1.66014,0.27996 2.2461,0.83984 0.59894,0.54689 0.89842,1.2435 0.89843,2.08985 -10e-6,0.85938 -0.306,1.59506 -0.91797,2.20703 -0.61199,0.59896 -1.35418,0.89844 -2.22656,0.89843 m 1.75781,-13.65234 c 2.35676,0.22138 4.41405,1.15237 6.17188,2.79297 2.2005,2.04429 3.30076,4.6159 3.30078,7.71484 -2e-5,3.11199 -1.09377,5.74871 -3.28125,7.91016 -2.18752,2.16146 -4.83726,3.24219 -7.94922,3.24219 -3.07293,0 -5.71615,-1.08073 -7.92969,-3.24219 -2.20052,-2.16145 -3.30078,-4.79817 -3.30078,-7.91016 0,-3.11196 1.10026,-5.68357 3.30078,-7.71484 1.77083,-1.6406 3.82812,-2.5781 6.17188,-2.8125 l -0.0195,-7.01172 -5.13672,5.11719 -0.0195,-3.88672 6.93359,-6.81641 6.9336,6.81641 0,3.88672 -5.17578,-5.09766 0,7.01172 m -1.75782,18.16406 c 2.1354,10e-6 3.95181,-0.74218 5.44922,-2.22656 1.51039,-1.48437 2.2656,-3.29426 2.26563,-5.42969 -3e-5,-2.12238 -0.75524,-3.8867 -2.26563,-5.29296 -1.49741,-1.40624 -3.31382,-2.10936 -5.44922,-2.10938 -2.09636,2e-5 -3.90626,0.70314 -5.42969,2.10938 -1.52344,1.40626 -2.28516,3.17058 -2.28515,5.29296 -10e-6,2.13543 0.76171,3.94532 2.28515,5.42969 1.52343,1.48438 3.33333,2.22657 5.42969,2.22656"/>
                <path
                    id="planet_neptune_tmpl"
                    class="planet-symbol"
                    d="m 14.33594,25.56641 0,3.65234 -2.94922,0 0,-3.65234 -3.4375,0 0,-2.94922 3.4375,0 0,-2.71484 c -2.14845,-0.28645 -4.01694,-1.22395 -5.60547,-2.8125 -1.95313,-1.95312 -2.93621,-4.32942 -2.94922,-7.12891 l 0,-1.64063 -2.8125,2.77344 -0.0195,-2.73437 2.83203,-2.98828 2.96875,0 2.8125,2.98828 0,2.73437 -2.8125,-2.77344 0,1.64063 0.0391,0 0,0.0391 -0.0391,0 c 0.013,1.96617 0.6966,3.62632 2.05078,4.98047 1.01561,1.02866 2.194,1.67319 3.53516,1.93359 l 0,-13.94531 -2.83203,2.75391 -0.0195,-2.71485 2.85156,-3.00781 2.94922,0 2.83203,3.00781 0,2.71485 -2.83203,-2.75391 0,13.92578 c 1.31508,-0.2604 2.47394,-0.89842 3.47656,-1.91406 1.38019,-1.38019 2.07029,-3.04686 2.07031,-5 l 0,-0.0195 0.0195,0 0,-1.73828 -2.8125,2.77344 -0.0195,-2.73438 2.83204,-2.98828 2.96875,0 2.8125,2.98828 0,2.73438 -2.8125,-2.77344 0,1.73828 0.0195,0 0,0.0781 c -0.013,2.74742 -0.99612,5.09768 -2.94922,7.05079 -1.58857,1.57553 -3.45705,2.51303 -5.60547,2.8125 l 0,2.71484 3.41797,0 0,2.94922 -3.41797,0"/>
                <path
                    id="planet_pluto_tmpl"
                    class="planet-symbol"
                    d="m 0.15625,0 7.98828,0 c 3.93227,3e-5 6.54945,1.3542 7.85156,4.0625 0.41664,1.00263 0.62498,2.12242 0.625,3.35938 -2e-5,3.46356 -1.63414,5.79428 -4.90234,6.99218 -0.76825,0.22137 -1.50393,0.33205 -2.20703,0.33204 l -6.67969,0 0,11.62109 12.38281,0 0.15625,0.15625 0,2.53906 -0.15625,0.15625 -15.05859,0 -0.15625,-0.15625 0,-28.88672 0.15625,-0.17578 m 2.67578,2.73438 0,9.29687 6.36719,0 c 1.69269,2e-5 3.00779,-0.84633 3.94531,-2.53906 0.20831,-0.33852 0.36456,-1.02863 0.46875,-2.07031 -2e-5,-2.1354 -1.05471,-3.59373 -3.16406,-4.375 -0.48179,-0.20831 -1.25002,-0.31248 -2.30469,-0.3125 l -5.3125,0"/>
                <path
                    id="planet_chiron_tmpl"
                    class="planet-symbol"
                    d="m 4.10156,22.94922 c -0.75522,0.76824 -1.13282,1.69271 -1.13281,2.77344 -1e-5,1.08073 0.37759,1.9987 1.13281,2.7539 0.76822,0.76823 1.6927,1.15235 2.77344,1.15235 1.08071,0 1.99868,-0.38412 2.75391,-1.15235 0.76821,-0.7552 1.15232,-1.67317 1.15234,-2.7539 -2e-5,-1.08073 -0.38413,-2.0052 -1.15234,-2.77344 -0.75523,-0.76822 -1.6732,-1.15234 -2.75391,-1.15234 -1.08074,0 -2.00522,0.38412 -2.77344,1.15234 m 2.77344,9.60938 c -1.88803,-1e-5 -3.50912,-0.67058 -4.86328,-2.01172 -1.34115,-1.34115 -2.01173,-2.95573 -2.01172,-4.84375 -1e-5,-1.90104 0.67708,-3.52213 2.03125,-4.86328 0.93749,-0.92447 2.01171,-1.52994 3.22266,-1.81641 l 0,-19.02344 3.35937,0 0,5.97657 5.72266,-3.37891 1.67969,2.89062 -7.01172,4.08204 7.01172,4.10156 -1.67969,2.89062 -5.72266,-3.39843 0,5.89843 c 1.15884,0.28647 2.194,0.87892 3.10547,1.77735 1.35415,1.34115 2.03123,2.96224 2.03125,4.86328 -2e-5,1.88802 -0.6771,3.5026 -2.03125,4.84375 -1.34116,1.34114 -2.95574,2.01171 -4.84375,2.01172"/>
                <path
                    id="planet_ceres_tmpl"
                    class="planet-symbol"
                    d="m 9.6875,29.25781 0,5.01953 -3.53516,0 0,-5.01953 -5.3125,0 0,-2.42187 5.3125,0 0,-8.47657 c 0.52082,0.0912 1.11978,0.13673 1.79687,0.13672 2.09634,1e-5 3.89972,-0.70311 5.41016,-2.10937 1.51039,-1.40624 2.2656,-3.17056 2.26563,-5.29297 -3e-5,-2.1354 -0.75524,-3.94529 -2.26563,-5.42969 -1.51044,-1.48435 -3.32684,-2.22654 -5.44922,-2.22656 -2.10939,2e-5 -3.98438,0.69013 -5.625,2.07031 l -2.28515,-2.26562 c 2.18749,-2.16143 4.8242,-3.24216 7.91015,-3.24219 3.09894,3e-5 5.74217,1.08076 7.92969,3.24219 2.18747,2.16148 3.28122,4.80471 3.28125,7.92968 -3e-5,3.12502 -1.08727,5.69012 -3.26172,7.69532 -1.78388,1.64063 -3.84116,2.57813 -6.17187,2.8125 l 0,5.15625 5.29296,0 0,2.42187 -5.29296,0"/>
                <path
                    id="planet_pallas_tmpl"
                    class="planet-symbol"
                    d="m 0,11.15234 11.21094,-11.15234 11.21094,11.15234 -9.35547,10.44922 0,5.23438 c 1.7578,0 3.52212,0 5.29297,0 l 0,2.42187 c -1.77085,0 -3.53517,0 -5.29297,0 l 0,5.01953 c -1.18491,0 -2.36329,0 -3.53516,0 l 0,-5.01953 c -1.77084,0 -3.54167,0 -5.3125,0 l 0,-2.42187 c 1.77083,0 3.54166,0 5.3125,0 l 0,-5.23438 -9.53125,-10.44922 m 11.21094,7.16797 6.48438,-7.16797 -6.48438,-6.44531 -6.48437,6.44531 6.48437,7.16797"/>
                <path
                    id="planet_juno_tmpl"
                    class="planet-symbol"
                    d="m 2.12891,4.39453 2.40234,-2.38281 4.94141,4.9414 0,-6.95312 3.49609,0 0,6.93359 4.92188,-4.92187 2.40234,2.38281 -5.54687,5.54688 7.67578,0 0,2.42187 -7.53906,0 5.41015,5.41016 -2.38281,2.40234 -4.94141,-4.94141 0,11.60157 5.3125,0 0,2.42187 -5.3125,0 0,5.01953 -3.51562,0 0,-5.01953 -5.3125,0 0,-2.42187 5.3125,0 0,-11.60157 -4.94141,4.94141 -2.38281,-2.40234 5.41016,-5.41016 -7.53907,0 0,-2.42187 7.67579,0 -5.54688,-5.54688"/>
                <path
                    id="planet_vesta_tmpl"
                    class="planet-symbol"
                    d="m 29.29688,19.27734 0,2.42187 -5.87891,0 -8.76953,8.73047 -8.76953,-8.73047 -5.87891,0 0,-2.42187 8.16407,0 6.48437,6.44531 6.48438,-6.44531 8.16406,0 m -12.87109,-19.27734 0,11.75781 -3.35938,0 0,-11.75781 3.35938,0 m -11.03516,12.73437 5.07812,0 4.17969,4.16016 4.17969,-4.16016 5.07812,0 0,2.42188 -2.77343,0 -6.48438,6.44531 -6.48437,-6.44531 -2.77344,0 0,-2.42188"/>
                <path
                    id="planet_moon_node_tmpl"
                    class="planet-symbol"
                    d="m 27.01172,20.54688 c -0.66409,-0.67708 -1.47789,-1.01562 -2.44141,-1.01563 -0.96356,1e-5 -1.78388,0.33855 -2.46093,1.01563 -0.66409,0.66407 -0.99612,1.47787 -0.9961,2.4414 -2e-5,0.95053 0.33201,1.76433 0.9961,2.44141 0.67705,0.66406 1.49737,0.99609 2.46093,0.99609 0.9505,0 1.7643,-0.33203 2.44141,-0.99609 0.67705,-0.67708 1.01559,-1.49088 1.01562,-2.44141 -3e-5,-0.96353 -0.33857,-1.77733 -1.01562,-2.4414 m -20.9375,-1.01563 c -0.95053,1e-5 -1.76433,0.33855 -2.44141,1.01563 -0.67709,0.66407 -1.01563,1.47787 -1.01562,2.4414 -10e-6,0.95053 0.33853,1.76433 1.01562,2.44141 0.67708,0.67708 1.49088,1.01563 2.44141,1.01562 0.96353,1e-5 1.77733,-0.33854 2.44141,-1.01562 0.67707,-0.67708 1.01561,-1.49088 1.01562,-2.44141 -10e-6,-0.96353 -0.33855,-1.77733 -1.01562,-2.4414 -0.66408,-0.67708 -1.47788,-1.01562 -2.44141,-1.01563 m 21.89453,-1.66016 c 0.32549,0.22137 0.63148,0.47528 0.91797,0.76172 1.18486,1.19793 1.77731,2.64324 1.77734,4.33594 -3e-5,1.67969 -0.59248,3.11849 -1.77734,4.31641 -1.18493,1.18489 -2.62373,1.77734 -4.31641,1.77734 -1.67971,0 -3.112,-0.59245 -4.29687,-1.77734 -1.18492,-1.19792 -1.77737,-2.63672 -1.77735,-4.31641 -2e-5,-1.6927 0.58592,-3.13801 1.75782,-4.33594 1.05466,-1.05467 2.30466,-1.64061 3.75,-1.75781 1.57549,-1.49738 2.36325,-3.24217 2.36328,-5.23437 -3e-5,-2.34373 -1.09378,-4.33592 -3.28125,-5.97657 -2.1745,-1.65362 -4.82424,-2.48044 -7.94922,-2.48047 -3.12501,3e-5 -5.78126,0.82685 -7.96875,2.48047 -2.17449,1.64065 -3.26173,3.63284 -3.26172,5.97657 -10e-6,1.9922 0.78775,3.73048 2.36328,5.21484 1.62759,0.0521 3.0078,0.64454 4.14063,1.77734 1.17186,1.19793 1.7578,2.64324 1.75781,4.33594 -1e-5,1.69271 -0.59246,3.13802 -1.77734,4.33594 -1.18491,1.18489 -2.62371,1.77734 -4.31641,1.77734 -1.66667,0 -3.09896,-0.59245 -4.29688,-1.77734 -1.18489,-1.19792 -1.77734,-2.64323 -1.77734,-4.33594 0,-1.6927 0.59245,-3.13801 1.77734,-4.33594 0.23438,-0.22134 0.47526,-0.42967 0.72266,-0.625 -1.58854,-1.84894 -2.38281,-3.95832 -2.38281,-6.32812 0,-3.22915 1.45833,-5.98305 4.375,-8.26172 2.91666,-2.27862 6.46483,-3.41794 10.64453,-3.41797 4.21873,3e-5 7.78643,1.13935 10.70312,3.41797 2.91664,2.27867 4.37497,5.03257 4.375,8.26172 -3e-5,2.3047 -0.74873,4.3685 -2.24609,6.1914"/>
                <path
                    id="planet_moon_apogee_tmpl"
                    class="planet-symbol"
                    d="m 10.07812,29.39453 0,5.01953 -3.51562,0 0,-5.01953 -5.3125,0 0,-2.42187 5.3125,0 0,-5.9961 c -2.03126,-0.98957 -3.71095,-2.47394 -5.03906,-4.45312 -1.01563,-1.52343 -1.52345,-3.33332 -1.52344,-5.42969 -1e-5,-3.93227 1.93358,-7.10284 5.80078,-9.51172 1.21092,-0.75518 2.9492,-1.23695 5.21484,-1.44531 0.93749,-0.0911 1.79035,0.013 2.5586,0.3125 -2.64325,0.76826 -4.60288,2.01826 -5.87891,3.75 -1.51043,2.04429 -2.26563,4.34247 -2.26562,6.89453 -10e-6,3.99741 1.62759,7.05079 4.88281,9.16016 1.13279,0.72917 2.25258,1.19141 3.35937,1.38672 -0.96356,0.48177 -2.16147,0.59896 -3.59375,0.35156 l 0,4.98047 5.3125,0 0,2.42187 -5.3125,0"/>
                <path
                    id="point_vertex_tmpl"
                    class="planet-symbol"
                    d="m 0,0 9.6800001,27.76 4.12,0 9.68,-27.76 -3.68,0 -7.92,24.24 -7.9200001,-24.24 -3.96,0 m 35.5768751,8.48 -5.24,7.64 -5.44,-7.64 -3.72,0 7,9.8 -6.76,9.48 3.4,0 4.92,-7.24 5.16,7.24 3.72,0 -6.72,-9.44 7.12,-9.84 -3.44,0" />
            </defs>
            <g id="chart" transform="translate(400,400)">
                <g id="moonless_chart">
                    <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(', $asc_rotate, ',0,0)')" /></xsl:attribute>
                    <g id="base">
                        <circle id="outer_circle" cx="0" cy="0" r="300" class="thick" />
                        <circle id="middle_circle" cx="0" cy="0" r="255" class="thin" />
                        <circle id="inner_circle" cx="0" cy="0" class="thick">
                            <xsl:attribute name="r"><xsl:value-of select="$r_aspect"/></xsl:attribute>
                        </circle>
                        <circle id="house_circle" cx="0" cy="0" r="70" class="thin" />
                        <circle id="moon_circle" cx="0" cy="0" r="50" class="thick" style="fill:#00000;stroke:none" />

                        <line id="aries_start" x1="240" y1="0" x2="300" y2="0" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="taurus_start" transform="rotate(-30,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="gemini_start" transform="rotate(-60,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="cancer_start" transform="rotate(-90,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="leo_start" transform="rotate(-120,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="virgo_start" transform="rotate(-150,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="libra_start" transform="rotate(-180,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="scorpio_start" transform="rotate(-210,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="sagittarius_start" transform="rotate(-240,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="capricorn_start" transform="rotate(-270,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="aquarius_start" transform="rotate(-300,0,0)" class="degree-thick" />
                        <use x="0" y="0" xlink:href="#aries_start" id="pisces_start" transform="rotate(-330,0,0)" class="degree-thick" />

                        <line id="deg_10" x1="240" y1="0" x2="255" y2="0" transform="rotate(-10,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_20" transform="rotate(-10,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_40" transform="rotate(-30,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_50" transform="rotate(-40,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_70" transform="rotate(-60,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_80" transform="rotate(-70,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_100" transform="rotate(-90,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_110" transform="rotate(-100,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_130" transform="rotate(-120,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_140" transform="rotate(-130,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_160" transform="rotate(-150,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_170" transform="rotate(-160,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_190" transform="rotate(-180,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_200" transform="rotate(-190,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_220" transform="rotate(-210,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_230" transform="rotate(-220,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_250" transform="rotate(-240,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_260" transform="rotate(-250,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_280" transform="rotate(-270,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_290" transform="rotate(-280,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_310" transform="rotate(-300,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_320" transform="rotate(-310,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_340" transform="rotate(-330,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_10" id="deg_350" transform="rotate(-340,0,0)" class="degree-thin" />

                        <line id="deg_5" x1="240" y1="0" x2="250" y2="0" transform="rotate(-5,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_15" transform="rotate(-10,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_25" transform="rotate(-20,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_35" transform="rotate(-30,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_45" transform="rotate(-40,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_55" transform="rotate(-50,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_65" transform="rotate(-60,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_75" transform="rotate(-70,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_85" transform="rotate(-80,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_95" transform="rotate(-90,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_105" transform="rotate(-100,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_115" transform="rotate(-110,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_125" transform="rotate(-120,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_135" transform="rotate(-130,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_145" transform="rotate(-140,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_155" transform="rotate(-150,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_165" transform="rotate(-160,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_175" transform="rotate(-170,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_185" transform="rotate(-180,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_195" transform="rotate(-190,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_205" transform="rotate(-200,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_215" transform="rotate(-210,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_225" transform="rotate(-220,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_235" transform="rotate(-230,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_245" transform="rotate(-240,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_255" transform="rotate(-250,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_265" transform="rotate(-260,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_275" transform="rotate(-270,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_285" transform="rotate(-280,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_295" transform="rotate(-290,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_305" transform="rotate(-300,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_315" transform="rotate(-310,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_325" transform="rotate(-320,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_335" transform="rotate(-330,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_345" transform="rotate(-340,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="#deg_355" transform="rotate(-350,0,0)" class="degree-thin" />

                        <line id="deg_1" x1="240" y1="0" x2="245" y2="0" transform="rotate(-1,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_2" transform="rotate(-1,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_3" transform="rotate(-2,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_4" transform="rotate(-3,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_6" transform="rotate(-5,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_7" transform="rotate(-6,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_8" transform="rotate(-7,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_9" transform="rotate(-8,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_11" transform="rotate(-10,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_12" transform="rotate(-11,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_13" transform="rotate(-12,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_14" transform="rotate(-13,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_16" transform="rotate(-15,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_17" transform="rotate(-16,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_18" transform="rotate(-17,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_19" transform="rotate(-18,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_21" transform="rotate(-20,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_22" transform="rotate(-21,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_23" transform="rotate(-22,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_24" transform="rotate(-23,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_26" transform="rotate(-25,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_27" transform="rotate(-26,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_28" transform="rotate(-27,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_29" transform="rotate(-28,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_31" transform="rotate(-30,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_32" transform="rotate(-31,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_33" transform="rotate(-32,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_34" transform="rotate(-33,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_36" transform="rotate(-35,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_37" transform="rotate(-36,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_38" transform="rotate(-37,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_39" transform="rotate(-38,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_41" transform="rotate(-40,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_42" transform="rotate(-41,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_43" transform="rotate(-42,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_44" transform="rotate(-43,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_46" transform="rotate(-45,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_47" transform="rotate(-46,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_48" transform="rotate(-47,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_49" transform="rotate(-48,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_51" transform="rotate(-50,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_52" transform="rotate(-51,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_53" transform="rotate(-52,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_54" transform="rotate(-53,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_56" transform="rotate(-55,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_57" transform="rotate(-56,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_58" transform="rotate(-57,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_59" transform="rotate(-58,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_61" transform="rotate(-60,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_62" transform="rotate(-61,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_63" transform="rotate(-62,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_64" transform="rotate(-63,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_66" transform="rotate(-65,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_67" transform="rotate(-66,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_68" transform="rotate(-67,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_69" transform="rotate(-68,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_71" transform="rotate(-70,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_72" transform="rotate(-71,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_73" transform="rotate(-72,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_74" transform="rotate(-73,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_76" transform="rotate(-75,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_77" transform="rotate(-76,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_78" transform="rotate(-77,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_79" transform="rotate(-78,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_81" transform="rotate(-80,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_82" transform="rotate(-81,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_83" transform="rotate(-82,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_84" transform="rotate(-83,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_86" transform="rotate(-85,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_87" transform="rotate(-86,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_88" transform="rotate(-87,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_89" transform="rotate(-88,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_91" transform="rotate(-90,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_92" transform="rotate(-91,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_93" transform="rotate(-92,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_94" transform="rotate(-93,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_96" transform="rotate(-95,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_97" transform="rotate(-96,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_98" transform="rotate(-97,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_99" transform="rotate(-98,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_101" transform="rotate(-100,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_102" transform="rotate(-101,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_103" transform="rotate(-102,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_104" transform="rotate(-103,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_106" transform="rotate(-105,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_107" transform="rotate(-106,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_108" transform="rotate(-107,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_109" transform="rotate(-108,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_111" transform="rotate(-110,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_112" transform="rotate(-111,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_113" transform="rotate(-112,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_114" transform="rotate(-113,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_116" transform="rotate(-115,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_117" transform="rotate(-116,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_118" transform="rotate(-117,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_119" transform="rotate(-118,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_121" transform="rotate(-120,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_122" transform="rotate(-121,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_123" transform="rotate(-122,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_124" transform="rotate(-123,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_126" transform="rotate(-125,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_127" transform="rotate(-126,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_128" transform="rotate(-127,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_129" transform="rotate(-128,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_131" transform="rotate(-130,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_132" transform="rotate(-131,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_133" transform="rotate(-132,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_134" transform="rotate(-133,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_136" transform="rotate(-135,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_137" transform="rotate(-136,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_138" transform="rotate(-137,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_139" transform="rotate(-138,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_141" transform="rotate(-140,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_142" transform="rotate(-141,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_143" transform="rotate(-142,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_144" transform="rotate(-143,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_146" transform="rotate(-145,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_147" transform="rotate(-146,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_148" transform="rotate(-147,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_149" transform="rotate(-148,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_151" transform="rotate(-150,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_152" transform="rotate(-151,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_153" transform="rotate(-152,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_154" transform="rotate(-153,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_156" transform="rotate(-155,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_157" transform="rotate(-156,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_158" transform="rotate(-157,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_159" transform="rotate(-158,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_161" transform="rotate(-160,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_162" transform="rotate(-161,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_163" transform="rotate(-162,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_164" transform="rotate(-163,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_166" transform="rotate(-165,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_167" transform="rotate(-166,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_168" transform="rotate(-167,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_169" transform="rotate(-168,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_171" transform="rotate(-170,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_172" transform="rotate(-171,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_173" transform="rotate(-172,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_174" transform="rotate(-173,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_176" transform="rotate(-175,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_177" transform="rotate(-176,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_178" transform="rotate(-177,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_179" transform="rotate(-178,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_181" transform="rotate(-180,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_182" transform="rotate(-181,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_183" transform="rotate(-182,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_184" transform="rotate(-183,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_186" transform="rotate(-185,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_187" transform="rotate(-186,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_188" transform="rotate(-187,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_189" transform="rotate(-188,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_191" transform="rotate(-190,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_192" transform="rotate(-191,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_193" transform="rotate(-192,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_194" transform="rotate(-193,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_196" transform="rotate(-195,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_197" transform="rotate(-196,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_198" transform="rotate(-197,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_199" transform="rotate(-198,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_201" transform="rotate(-200,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_202" transform="rotate(-201,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_203" transform="rotate(-202,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_204" transform="rotate(-203,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_206" transform="rotate(-205,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_207" transform="rotate(-206,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_208" transform="rotate(-207,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_209" transform="rotate(-208,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_211" transform="rotate(-210,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_212" transform="rotate(-211,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_213" transform="rotate(-212,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_214" transform="rotate(-213,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_216" transform="rotate(-215,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_217" transform="rotate(-216,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_218" transform="rotate(-217,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_219" transform="rotate(-218,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_221" transform="rotate(-220,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_222" transform="rotate(-221,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_223" transform="rotate(-222,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_224" transform="rotate(-223,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_226" transform="rotate(-225,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_227" transform="rotate(-226,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_228" transform="rotate(-227,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_229" transform="rotate(-228,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_231" transform="rotate(-230,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_232" transform="rotate(-231,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_233" transform="rotate(-232,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_234" transform="rotate(-233,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_236" transform="rotate(-235,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_237" transform="rotate(-236,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_238" transform="rotate(-237,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_239" transform="rotate(-238,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_241" transform="rotate(-240,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_242" transform="rotate(-241,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_243" transform="rotate(-242,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_244" transform="rotate(-243,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_246" transform="rotate(-245,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_247" transform="rotate(-246,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_248" transform="rotate(-247,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_249" transform="rotate(-248,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_251" transform="rotate(-250,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_252" transform="rotate(-251,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_253" transform="rotate(-252,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_254" transform="rotate(-253,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_256" transform="rotate(-255,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_257" transform="rotate(-256,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_258" transform="rotate(-257,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_259" transform="rotate(-258,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_261" transform="rotate(-260,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_262" transform="rotate(-261,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_263" transform="rotate(-262,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_264" transform="rotate(-263,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_266" transform="rotate(-265,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_267" transform="rotate(-266,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_268" transform="rotate(-267,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_269" transform="rotate(-268,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_271" transform="rotate(-270,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_272" transform="rotate(-271,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_273" transform="rotate(-272,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_274" transform="rotate(-273,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_276" transform="rotate(-275,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_277" transform="rotate(-276,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_278" transform="rotate(-277,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_279" transform="rotate(-278,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_281" transform="rotate(-280,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_282" transform="rotate(-281,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_283" transform="rotate(-282,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_284" transform="rotate(-283,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_286" transform="rotate(-285,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_287" transform="rotate(-286,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_288" transform="rotate(-287,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_289" transform="rotate(-288,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_291" transform="rotate(-290,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_292" transform="rotate(-291,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_293" transform="rotate(-292,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_294" transform="rotate(-293,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_296" transform="rotate(-295,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_297" transform="rotate(-296,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_298" transform="rotate(-297,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_299" transform="rotate(-298,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_301" transform="rotate(-300,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_302" transform="rotate(-301,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_303" transform="rotate(-302,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_304" transform="rotate(-303,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_306" transform="rotate(-305,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_307" transform="rotate(-306,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_308" transform="rotate(-307,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_309" transform="rotate(-308,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_311" transform="rotate(-310,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_312" transform="rotate(-311,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_313" transform="rotate(-312,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_314" transform="rotate(-313,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_316" transform="rotate(-315,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_317" transform="rotate(-316,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_318" transform="rotate(-317,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_319" transform="rotate(-318,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_321" transform="rotate(-320,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_322" transform="rotate(-321,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_323" transform="rotate(-322,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_324" transform="rotate(-323,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_326" transform="rotate(-325,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_327" transform="rotate(-326,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_328" transform="rotate(-327,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_329" transform="rotate(-328,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_331" transform="rotate(-330,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_332" transform="rotate(-331,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_333" transform="rotate(-332,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_334" transform="rotate(-333,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_336" transform="rotate(-335,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_337" transform="rotate(-336,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_338" transform="rotate(-337,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_339" transform="rotate(-338,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_341" transform="rotate(-340,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_342" transform="rotate(-341,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_343" transform="rotate(-342,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_344" transform="rotate(-343,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_346" transform="rotate(-345,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_347" transform="rotate(-346,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_348" transform="rotate(-347,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_349" transform="rotate(-348,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_351" transform="rotate(-350,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_352" transform="rotate(-351,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_353" transform="rotate(-352,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_354" transform="rotate(-353,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_356" transform="rotate(-355,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_357" transform="rotate(-356,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_358" transform="rotate(-357,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_1" id="deg_359" transform="rotate(-358,0,0)" class="degree-thin" />

                        <use x="0" y="0" xlink:href="#sign_aries_tmpl" id="sign_aries" transform="rotate(-15,0,0) translate(261,-15) rotate(90,15,15)" class="sign sign-fire" />
                        <use x="0" y="0" xlink:href="#sign_taurus_tmpl" id="sign_taurus" transform="rotate(-45,0,0) translate(261,-15) rotate(90,15,15)" class="sign sign-earth" />
                        <use x="0" y="0" xlink:href="#sign_gemini_tmpl" id="sign_gemini" transform="rotate(-75,0,0) translate(261,-15) rotate(90,15,15)" class="sign sign-air" />
                        <use x="0" y="0" xlink:href="#sign_cancer_tmpl" id="sign_cancer" transform="rotate(-105,0,0) translate(261,-14) rotate(90,15,14)" class="sign sign-water" />
                        <use x="0" y="0" xlink:href="#sign_leo_tmpl" id="sign_leo" transform="rotate(-135,0,0) translate(264,-15) rotate(90,12,15)" class="sign sign-fire" />
                        <use x="0" y="0" xlink:href="#sign_virgo_tmpl" id="sign_virgo" transform="rotate(-165,0,0) translate(262,-18) rotate(90,15,18)" class="sign sign-earth" />
                        <use x="0" y="0" xlink:href="#sign_libra_tmpl" id="sign_libra" transform="rotate(-195,0,0) translate(256,-18) rotate(90,15,18)" class="sign sign-air" />
                        <use x="0" y="0" xlink:href="#sign_scorpio_tmpl" id="sign_scorpio" transform="rotate(-225,0,0) translate(260,-18) rotate(90,15,18)" class="sign sign-water" />
                        <use x="0" y="0" xlink:href="#sign_sagittarius_tmpl" id="sign_sagittarius" transform="rotate(-255,0,0) translate(260,-18) rotate(90,15,18)" class="sign sign-fire" />
                        <use x="0" y="0" xlink:href="#sign_capricorn_tmpl" id="sign_capricorn" transform="rotate(-285,0,0) translate(259,-18) rotate(90,15,18)" class="sign sign-earth" />
                        <use x="0" y="0" xlink:href="#sign_aquarius_tmpl" id="sign_aquarius" transform="rotate(-315,0,0) translate(253,-18) rotate(90,15,18)" class="sign sign-air" />
                        <use x="0" y="0" xlink:href="#sign_pisces_tmpl" id="sign_pisces" transform="rotate(-345,0,0) translate(259,-18) rotate(90,15,18)" class="sign sign-water" />

                        <xsl:for-each select="chartinfo/houses/house">
                            <line x1="50" y1="0" x2="240" y2="0" class="house-cusp">
                                <xsl:attribute name="id"><xsl:value-of select="concat('house_cusp_', @number)"/></xsl:attribute>
                                <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', @degree, ')')" /></xsl:attribute>
                            </line>
                            <line x1="300" y1="0" x2="330" y2="0" class="house-cusp">
                                <xsl:attribute name="id"><xsl:value-of select="concat('house_cusp_', @number, '_outer')"/></xsl:attribute>
                                <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', @degree, ')')" /></xsl:attribute>
                            </line>
                        </xsl:for-each>

                        <line id="descendent" x1="-320" y1="0" x2="-50" y2="0" transform="rotate(-103.432962,0,0)" class="axis">
                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', $asc, ')')" /></xsl:attribute>
                        </line>
                        <line id="ascendent" x1="50" y1="0" x2="320" y2="0" class="axis axis-end">
                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', $asc, ')')" /></xsl:attribute>
                        </line>
                        <xsl:variable name="mc" select="chartinfo/ascmcs/mc/@degree_ut"/>
                        <line id="ic" x1="-320" y1="0" x2="-50" y2="0" class="axis">
                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', $mc, ')')" /></xsl:attribute>
                        </line>
                        <line id="mc" x1="50" y1="0" x2="320" y2="0" class="axis axis-end">
                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', $mc, ')')" /></xsl:attribute>
                        </line>

                        <g id="planets">
                            <xsl:for-each select="chartinfo/ascmcs/vertex">
                                <xsl:variable name="planet_base">point_vertex</xsl:variable>
                                <xsl:variable name="degree"><xsl:value-of select="@degree_ut" /></xsl:variable>
                                <xsl:variable name="negative_degree"><xsl:value-of select="0 - $degree" /></xsl:variable>
                                <g>
                                    <xsl:attribute name="id"><xsl:value-of select="$planet_base"/></xsl:attribute>
                                    <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(', $negative_degree, ',0,0)')"/></xsl:attribute>
                                    <line x1="230" y1="0" x2="240" y2="0" class="planet-marker">
                                        <xsl:attribute name="id"><xsl:value-of select="concat('mark_', $planet_base)" /></xsl:attribute>
                                    </line>
                                    <line x1="300" y1="0" x2="310" y2="0" class="planet-marker">
                                        <xsl:attribute name="id"><xsl:value-of select="concat('mark_', $planet_base, '_outer')" /></xsl:attribute>
                                    </line>
                                    <use>
                                        <xsl:attribute name="xlink:href"><xsl:value-of select="concat('#', $planet_base, '_tmpl')"/></xsl:attribute>
                                        <xsl:attribute name="transform"><xsl:value-of select="concat('translate(330,-15) rotate(', $degree - $asc_rotate ,',15,15)')"/></xsl:attribute>
                                    </use>
                                </g>
                            </xsl:for-each>
                            <xsl:for-each select="chartinfo/bodies/body">
                                <xsl:variable name="planet_base" select="substring(translate(@name, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), 6)"/>
                                <xsl:variable name="negative_degree"><xsl:value-of select="0 - @degree"/></xsl:variable>
                                <g>
                                    <xsl:attribute name="id"><xsl:value-of select="$planet_base"/></xsl:attribute>
                                    <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(', $negative_degree, ',0,0)')"/></xsl:attribute>
                                    <line x1="230" y1="0" x2="240" y2="0" class="planet-marker">
                                        <xsl:attribute name="id"><xsl:value-of select="concat('mark_', $planet_base)" /></xsl:attribute>
                                    </line>
                                    <line x1="300" y1="0" x2="310" y2="0" class="planet-marker">
                                        <xsl:attribute name="id"><xsl:value-of select="concat('mark_', $planet_base, '_outer')" /></xsl:attribute>
                                    </line>
                                    <use>
                                        <xsl:attribute name="xlink:href"><xsl:value-of select="concat('#', $planet_base, '_tmpl')"/></xsl:attribute>
                                        <xsl:attribute name="transform"><xsl:value-of select="concat('translate(330,-15) rotate(', @degree - $asc_rotate ,',15,15)')"/></xsl:attribute>
                                    </use>
                                </g>
                                <xsl:choose>
                                    <xsl:when test="@name='GSWE_PLANET_MOON_NODE'">
                                        <g>
                                            <xsl:attribute name="id"><xsl:value-of select="concat($planet_base, '_desc')"/></xsl:attribute>
                                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(', 180 + $negative_degree, ',0,0)')"/></xsl:attribute>
                                            <line x1="230" y1="0" x2="240" y2="0" class="planet-marker">
                                                <xsl:attribute name="id"><xsl:value-of select="concat('mark_', $planet_base)" /></xsl:attribute>
                                            </line>
                                            <line x1="300" y1="0" x2="310" y2="0" class="planet-marker">
                                                <xsl:attribute name="id"><xsl:value-of select="concat('mark_', $planet_base, '_outer')" /></xsl:attribute>
                                            </line>
                                            <use>
                                                <xsl:attribute name="xlink:href"><xsl:value-of select="concat('#', $planet_base, '_tmpl')"/></xsl:attribute>
                                                <xsl:attribute name="transform"><xsl:value-of select="concat('translate(330,-15) rotate(', @degree - $asc_rotate ,',15,15)')"/></xsl:attribute>
                                            </use>
                                        </g>
                                    </xsl:when>
                                </xsl:choose>
                            </xsl:for-each>
                        </g>

                        <g id="aspects">
                            <xsl:for-each select="chartinfo/aspects/aspect">
                                <xsl:variable name="planet1" select="@body1"/>
                                <xsl:variable name="deg1">
                                    <xsl:choose>
                                        <xsl:when test="$planet1='GSWE_PLANET_ASCENDENT'">
                                            <xsl:value-of select="/chartinfo/ascmcs/ascendant/@degree_ut" />
                                        </xsl:when>
                                        <xsl:when test="$planet1='GSWE_PLANET_MC'">
                                            <xsl:value-of select="/chartinfo/ascmcs/mc/@degree_ut" />
                                        </xsl:when>
                                        <xsl:when test="$planet1='GSWE_PLANET_VERTEX'">
                                            <xsl:value-of select="/chartinfo/ascmcs/vertex/@degree_ut" />
                                        </xsl:when>
                                        <xsl:otherwise>
                                            <xsl:value-of select="/chartinfo/bodies/body[@name=$planet1]/@degree" />
                                        </xsl:otherwise>
                                    </xsl:choose>
                                </xsl:variable>
                                <xsl:variable name="rad1" select="$deg1 * $PI div 180"/>
                                <xsl:variable name="x1" select="$r_aspect * math:cos($rad1)"/>
                                <xsl:variable name="y1" select="$r_aspect * -math:sin($rad1)"/>

                                <xsl:variable name="planet2" select="@body2"/>
                                <xsl:variable name="deg2">
                                    <xsl:choose>
                                        <xsl:when test="$planet2='GSWE_PLANET_ASCENDENT'">
                                            <xsl:value-of select="/chartinfo/ascmcs/ascendant/@degree_ut" />
                                        </xsl:when>
                                        <xsl:when test="$planet2='GSWE_PLANET_MC'">
                                            <xsl:value-of select="/chartinfo/ascmcs/mc/@degree_ut" />
                                        </xsl:when>
                                        <xsl:when test="$planet2='GSWE_PLANET_VERTEX'">
                                            <xsl:value-of select="/chartinfo/ascmcs/vertex/@degree_ut" />
                                        </xsl:when>
                                        <xsl:otherwise>
                                            <xsl:value-of select="/chartinfo/bodies/body[@name=$planet2]/@degree" />
                                        </xsl:otherwise>
                                    </xsl:choose>
                                </xsl:variable>
                                <xsl:variable name="rad2" select="$deg2 * $PI div 180"/>
                                <xsl:variable name="x2" select="$r_aspect * math:cos($rad2)"/>
                                <xsl:variable name="y2" select="$r_aspect * -math:sin($rad2)"/>

                                <line style="stroke-width:1;stroke:#000000;stroke-dasharray:20,10">
                                    <xsl:attribute name="x1"><xsl:value-of select="$x1"/></xsl:attribute>
                                    <xsl:attribute name="y1"><xsl:value-of select="$y1"/></xsl:attribute>
                                    <xsl:attribute name="x2"><xsl:value-of select="$x2"/></xsl:attribute>
                                    <xsl:attribute name="y2"><xsl:value-of select="$y2"/></xsl:attribute>
                                </line>
                            </xsl:for-each>
                        </g>

                        <g id="antiscia" style="visibility:hidden">
                            <xsl:for-each select="chartinfo/antiscia/antiscia">
                                <xsl:variable name="planet1" select="@body1"/>
                                <xsl:variable name="deg1">
                                    <xsl:choose>
                                        <xsl:when test="$planet1='GSWE_PLANET_ASCENDENT'">
                                            <xsl:value-of select="/chartinfo/ascmcs/ascendant/@degree_ut" />
                                        </xsl:when>
                                        <xsl:when test="$planet1='GSWE_PLANET_MC'">
                                            <xsl:value-of select="/chartinfo/ascmcs/mc/@degree_ut" />
                                        </xsl:when>
                                        <xsl:when test="$planet1='GSWE_PLANET_VERTEX'">
                                            <xsl:value-of select="/chartinfo/ascmcs/vertex/@degree_ut" />
                                        </xsl:when>
                                        <xsl:otherwise>
                                            <xsl:value-of select="/chartinfo/bodies/body[@name=$planet1]/@degree" />
                                        </xsl:otherwise>
                                    </xsl:choose>
                                </xsl:variable>
                                <xsl:variable name="rad1" select="$deg1 * $PI div 180"/>
                                <xsl:variable name="x1" select="$r_aspect * math:cos($rad1)"/>
                                <xsl:variable name="y1" select="$r_aspect * -math:sin($rad1)"/>

                                <xsl:variable name="planet2" select="@body2"/>
                                <xsl:variable name="deg2">
                                    <xsl:choose>
                                        <xsl:when test="$planet2='GSWE_PLANET_ASCENDENT'">
                                            <xsl:value-of select="/chartinfo/ascmcs/ascendant/@degree_ut" />
                                        </xsl:when>
                                        <xsl:when test="$planet2='GSWE_PLANET_MC'">
                                            <xsl:value-of select="/chartinfo/ascmcs/mc/@degree_ut" />
                                        </xsl:when>
                                        <xsl:when test="$planet2='GSWE_PLANET_VERTEX'">
                                            <xsl:value-of select="/chartinfo/ascmcs/vertex/@degree_ut" />
                                        </xsl:when>
                                        <xsl:otherwise>
                                            <xsl:value-of select="/chartinfo/bodies/body[@name=$planet2]/@degree" />
                                        </xsl:otherwise>
                                    </xsl:choose>
                                </xsl:variable>
                                <xsl:variable name="rad2" select="$deg2 * $PI div 180"/>
                                <xsl:variable name="x2" select="$r_aspect * math:cos($rad2)"/>
                                <xsl:variable name="y2" select="$r_aspect * -math:sin($rad2)"/>

                                <line style="stroke-width:1;stroke:#000000;stroke-dasharray:20,10">
                                    <xsl:attribute name="x1"><xsl:value-of select="$x1"/></xsl:attribute>
                                    <xsl:attribute name="y1"><xsl:value-of select="$y1"/></xsl:attribute>
                                    <xsl:attribute name="x2"><xsl:value-of select="$x2"/></xsl:attribute>
                                    <xsl:attribute name="y2"><xsl:value-of select="$y2"/></xsl:attribute>
                                </line>
                            </xsl:for-each>
                        </g>
                    </g>
                </g>
            </g>
        </svg>
    </xsl:template>
</xsl:stylesheet>
