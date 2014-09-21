<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:xi="http://www.w3.org/2001/XInclude"
    xmlns:xlink="http://www.w3.org/1999/xlink"
    xmlns:math="http://exslt.org/math">
    <xsl:output
        method="xml"
        media-type="image/svg+xml"
        doctype-public="-//W3C//DTD SVG 1.0//EN"
        doctype-system="http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd"
        version="1.0"
        encoding="UTF-8"
        indent="yes"/>
    <xsl:variable name="asc" select="chartinfo/ascmcs/ascendant/@degree_ut" />
    <xsl:variable name="asc_rotate" select="$asc - 180"/>
    <xsl:variable name="PI" select="math:constant('PI', 10)" />
    <xsl:variable name="image_size" select="800" />
    <xsl:variable name="icon_size" select="30" />
    <xsl:variable name="r_aspect" select="$image_size * 0.3" />

    <xsl:template name="planet-template">
        <xsl:param name="planet_name"/>
        <xsl:param name="planet_base"/>
        <xsl:param name="rotate"/>
        <xsl:param name="dist"/>
        <xsl:param name="retrograde"/>
        <xsl:param name="upside-down"/>

        <g xmlns="http://www.w3.org/2000/svg">
            <xsl:attribute name="id">planet-<xsl:value-of select="$planet_name"/></xsl:attribute>
            <xsl:attribute name="class">planet planet-<xsl:value-of select="$planet_name"/></xsl:attribute>
            <xsl:attribute name="transform">rotate(<xsl:value-of select="-$rotate"/>, 0, 0)</xsl:attribute>
            <line y1="0" y2="0" class="planet-marker">
                <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.2875"/></xsl:attribute>
                <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.3"/></xsl:attribute>
            </line>
            <line y1="0" y2="0" class="planet-marker">
                <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.375"/></xsl:attribute>
                <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.3875"/></xsl:attribute>
            </line>
            <g>
              <xsl:attribute name="transform">translate(<xsl:value-of select="$image_size * 0.4125 + $dist * ($icon_size * 1.1666666)"/>, <xsl:value-of select="-$icon_size div 2"/>) rotate(<xsl:value-of select="$rotate - $asc_rotate"/>, <xsl:value-of select="$icon_size div 2"/>, <xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                <use class="planet-symbol">
                    <xsl:attribute name="xlink:href">#<xsl:value-of select="$planet_base"/>_tmpl</xsl:attribute>
                    <xsl:choose>
                        <xsl:when test="$upside-down='yes'">
                            <xsl:attribute name="transform">rotate(180, <xsl:value-of select="$icon_size div 2"/>, <xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </xsl:when>
                    </xsl:choose>
                </use>
                <xsl:choose>
                    <xsl:when test="$retrograde='True'">
                        <text>
                            <xsl:attribute name="font-size"><xsl:value-of select="$icon_size div 2"/></xsl:attribute>
                            <xsl:attribute name="transform">translate(<xsl:value-of select="$icon_size"/>, <xsl:value-of select="$icon_size * 1.5"/>)</xsl:attribute>
                            R
                        </text>
                    </xsl:when>
                </xsl:choose>
            </g>
        </g>
    </xsl:template>

    <xsl:template match="/">
        <svg
            xmlns="http://www.w3.org/2000/svg"
            xmlns:xlink="http://www.w3.org/1999/xlink"
            version="1.0">
            <xsl:attribute name="width"><xsl:value-of select="$image_size"/></xsl:attribute>
            <xsl:attribute name="height"><xsl:value-of select="$image_size"/></xsl:attribute>

            <xsl:choose>
              <xsl:when test="$rendering='yes'">
                <style type="text/css">
                  <xi:include href="gres://ui/chart-default.css" parse="text"/>
                </style>
                <style type="text/css">
                  <xsl:value-of select="$additional-css"/>
                </style>
              </xsl:when>
            </xsl:choose>

            <title>
                <xsl:value-of select="concat(
                    'Natal chart of ',
                    chartinfo/data/name/text()
                    )"/>
            </title>
            <defs>
                <xi:include href="gres://default-icons/sign-aries.xml" />
                <xi:include href="gres://default-icons/sign-taurus.xml" />
                <xi:include href="gres://default-icons/sign-gemini.xml" />
                <xi:include href="gres://default-icons/sign-cancer.xml" />
                <xi:include href="gres://default-icons/sign-leo.xml" />
                <xi:include href="gres://default-icons/sign-virgo.xml" />
                <xi:include href="gres://default-icons/sign-libra.xml" />
                <xi:include href="gres://default-icons/sign-scorpio.xml" />
                <xi:include href="gres://default-icons/sign-sagittarius.xml" />
                <xi:include href="gres://default-icons/sign-capricorn.xml" />
                <xi:include href="gres://default-icons/sign-aquarius.xml" />
                <xi:include href="gres://default-icons/sign-pisces.xml" />

                <xi:include href="gres://default-icons/planet-sun.xml" />
                <xi:include href="gres://default-icons/planet-moon.xml" />
                <xi:include href="gres://default-icons/planet-mercury.xml" />
                <xi:include href="gres://default-icons/planet-venus.xml" />
                <xi:include href="gres://default-icons/planet-mars.xml" />
                <xi:include href="gres://default-icons/planet-jupiter.xml" />
                <xi:include href="gres://default-icons/planet-saturn.xml" />
                <xi:include href="gres://default-icons/planet-uranus.xml" />
                <xi:include href="gres://default-icons/planet-neptune.xml" />
                <xi:include href="gres://default-icons/planet-pluto.xml" />
                <xi:include href="gres://default-icons/planet-chiron.xml" />
                <xi:include href="gres://default-icons/planet-pholus.xml" />
                <xi:include href="gres://default-icons/planet-nessus.xml" />
                <xi:include href="gres://default-icons/planet-ceres.xml" />
                <xi:include href="gres://default-icons/planet-pallas.xml" />
                <xi:include href="gres://default-icons/planet-juno.xml" />
                <xi:include href="gres://default-icons/planet-vesta.xml" />
                <xi:include href="gres://default-icons/planet-chariklo.xml" />
                <xi:include href="gres://default-icons/planet-asbolus.xml" />
                <xi:include href="gres://default-icons/planet-moon-node.xml" />
                <xi:include href="gres://default-icons/planet-moon-apogee.xml" />
                <xi:include href="gres://default-icons/point-vertex.xml" />

                <!-- TODO: the size of this arrow should also depend on $image_size -->
                <marker id="arrow_end" orient="auto" refX="3.5" refY="0.0" style="overflow:visible">
                    <polygon points="0.0,0.0 7.0,-2.0 5.0,0.0 7.0,2.0" />
                </marker>
            </defs>

            <xsl:choose>
              <xsl:when test="$rendering='yes'">
                <rect id="background" x="0" y="0">
                  <xsl:attribute name="width"><xsl:value-of select="$image_size"/></xsl:attribute>
                  <xsl:attribute name="height"><xsl:value-of select="$image_size"/></xsl:attribute>
                </rect>
              </xsl:when>
            </xsl:choose>

            <g id="chart">
                <xsl:attribute name="transform"><xsl:value-of select="concat('translate(', $image_size div 2, ',', $image_size div 2, ')')" /></xsl:attribute>
                <g id="moonless_chart">
                    <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(', $asc_rotate, ',0,0)')" /></xsl:attribute>
                    <g id="base">
                        <circle id="outer_circle" cx="0" cy="0" class="thick">
                            <xsl:attribute name="r"><xsl:value-of select="$image_size * 0.375"/></xsl:attribute>
                        </circle>
                        <circle id="middle_circle" cx="0" cy="0" class="thin">
                            <xsl:attribute name="r"><xsl:value-of select="$image_size * 0.319"/></xsl:attribute>
                        </circle>
                        <circle id="inner_circle" cx="0" cy="0" class="thick">
                            <xsl:attribute name="r"><xsl:value-of select="$r_aspect"/></xsl:attribute>
                        </circle>
                        <circle id="house_circle" cx="0" cy="0" class="thin">
                            <xsl:attribute name="r"><xsl:value-of select="$image_size * 0.0875"/></xsl:attribute>
                        </circle>
                        <circle id="moon_circle" cx="0" cy="0" class="thick" style="fill:#00000;stroke:none">
                            <xsl:attribute name="r"><xsl:value-of select="$image_size * 0.0625"/></xsl:attribute>
                        </circle>

                        <line id="aries_start" y1="0" y2="0" class="degree-thick">
                            <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.3"/></xsl:attribute>
                            <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.375"/></xsl:attribute>
                        </line>
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

                        <line id="deg_10" y1="0" y2="0" transform="rotate(-10,0,0)" class="degree-thin">
                            <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.3"/></xsl:attribute>
                            <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.31875"/></xsl:attribute>
                        </line>
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

                        <line id="deg_5" y1="0" y2="0" transform="rotate(-5,0,0)" class="degree-thin">
                            <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.3"/></xsl:attribute>
                            <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.3125"/></xsl:attribute>
                        </line>
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_15" transform="rotate(-10,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_25" transform="rotate(-20,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_35" transform="rotate(-30,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_45" transform="rotate(-40,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_55" transform="rotate(-50,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_65" transform="rotate(-60,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_75" transform="rotate(-70,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_85" transform="rotate(-80,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_95" transform="rotate(-90,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_105" transform="rotate(-100,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_115" transform="rotate(-110,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_125" transform="rotate(-120,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_135" transform="rotate(-130,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_145" transform="rotate(-140,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_155" transform="rotate(-150,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_165" transform="rotate(-160,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_175" transform="rotate(-170,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_185" transform="rotate(-180,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_195" transform="rotate(-190,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_205" transform="rotate(-200,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_215" transform="rotate(-210,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_225" transform="rotate(-220,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_235" transform="rotate(-230,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_245" transform="rotate(-240,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_255" transform="rotate(-250,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_265" transform="rotate(-260,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_275" transform="rotate(-270,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_285" transform="rotate(-280,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_295" transform="rotate(-290,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_305" transform="rotate(-300,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_315" transform="rotate(-310,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_325" transform="rotate(-320,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_335" transform="rotate(-330,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_345" transform="rotate(-340,0,0)" class="degree-thin" />
                        <use x="0" y="0" xlink:href="#deg_5" id="deg_355" transform="rotate(-350,0,0)" class="degree-thin" />

                        <line id="deg_1" y1="0" y2="0" transform="rotate(-1,0,0)" class="degree-thin">
                            <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.3"/></xsl:attribute>
                            <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.30625"/></xsl:attribute>
                        </line>
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

                        <use x="0" y="0" xlink:href="#sign_aries_tmpl" id="sign_aries" class="sign sign-fire">
                            <xsl:attribute name="transform">rotate(-15,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_taurus_tmpl" id="sign_taurus" class="sign sign-earth">
                            <xsl:attribute name="transform">rotate(-45,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_gemini_tmpl" id="sign_gemini" class="sign sign-air">
                            <xsl:attribute name="transform">rotate(-75,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_cancer_tmpl" id="sign_cancer" class="sign sign-water">
                            <xsl:attribute name="transform">rotate(-105,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_leo_tmpl" id="sign_leo" class="sign sign-fire">
                            <xsl:attribute name="transform">rotate(-135,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_virgo_tmpl" id="sign_virgo" class="sign sign-earth">
                            <xsl:attribute name="transform">rotate(-165,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_libra_tmpl" id="sign_libra" class="sign sign-air">
                            <xsl:attribute name="transform">rotate(-195,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_scorpio_tmpl" id="sign_scorpio" class="sign sign-water">
                            <xsl:attribute name="transform">rotate(-225,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_sagittarius_tmpl" id="sign_sagittarius" class="sign sign-fire">
                            <xsl:attribute name="transform">rotate(-255,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_capricorn_tmpl" id="sign_capricorn" class="sign sign-earth">
                            <xsl:attribute name="transform">rotate(-285,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_aquarius_tmpl" id="sign_aquarius" class="sign sign-air">
                            <xsl:attribute name="transform">rotate(-315,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                        <use x="0" y="0" xlink:href="#sign_pisces_tmpl" id="sign_pisces" class="sign sign-water">
                            <xsl:attribute name="transform">rotate(-345,0,0) translate(<xsl:value-of select="$image_size * 0.32625"/>,-<xsl:value-of select="$icon_size div 2"/>) rotate(90,<xsl:value-of select="$icon_size div 2"/>,<xsl:value-of select="$icon_size div 2"/>)</xsl:attribute>
                        </use>
                    </g>

                    <g id="houes">
                        <xsl:for-each select="chartinfo/houses/house">
                            <xsl:variable name="next_house" select="@number + 1"/>
                            <xsl:variable name="next_degree_read">
                                <xsl:choose>
                                    <xsl:when test="/chartinfo/houses/house[@number=$next_house]">
                                        <xsl:value-of select="/chartinfo/houses/house[@number=$next_house]/@degree"/>
                                    </xsl:when>
                                    <xsl:otherwise>
                                        <xsl:value-of select="/chartinfo/houses/house[@number='1']/@degree"/>
                                    </xsl:otherwise>
                                </xsl:choose>
                            </xsl:variable>
                            <xsl:variable name="next_degree">
                                <xsl:choose>
                                    <xsl:when test="$next_degree_read &lt; @degree">
                                        <xsl:value-of select="$next_degree_read + 360.0"/>
                                    </xsl:when>
                                    <xsl:otherwise>
                                        <xsl:value-of select="$next_degree_read"/>
                                    </xsl:otherwise>
                                </xsl:choose>
                            </xsl:variable>
                            <xsl:variable name="house_mid_test" select="(@degree + $next_degree) div 2"/>
                            <xsl:variable name="house_mid">
                                <xsl:choose>
                                    <xsl:when test="$house_mid_test &gt; 360.0">
                                        <xsl:value-of select="$house_mid_test - 360.0"/>
                                    </xsl:when>
                                    <xsl:otherwise>
                                        <xsl:value-of select="$house_mid_test"/>
                                    </xsl:otherwise>
                                </xsl:choose>
                            </xsl:variable>
                            <text text-anchor="middle">
                                <xsl:attribute name="font-size"><xsl:value-of select="$image_size * 0.0125"/></xsl:attribute>
                                <xsl:attribute name="transform">
                                    rotate(-<xsl:value-of select="$house_mid"/>,0,0) translate(<xsl:value-of select="$image_size * 0.06875"/>,0) rotate(90,0,0)
                                </xsl:attribute>
                                <xsl:value-of select="@number"/>
                            </text>
                            <line y1="0" y2="0" class="house-cusp">
                                <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.0625"/></xsl:attribute>
                                <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.3"/></xsl:attribute>
                                <xsl:attribute name="id"><xsl:value-of select="concat('house_cusp_', @number)"/></xsl:attribute>
                                <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', @degree, ')')" /></xsl:attribute>
                            </line>
                            <line y1="0" y2="0" class="house-cusp">
                                <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.375"/></xsl:attribute>
                                <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.4125"/></xsl:attribute>
                                <xsl:attribute name="id"><xsl:value-of select="concat('house_cusp_', @number, '_outer')"/></xsl:attribute>
                                <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', @degree, ')')" /></xsl:attribute>
                            </line>
                        </xsl:for-each>

                        <line id="descendent" y1="0" y2="0" class="axis">
                            <xsl:attribute name="x1"><xsl:value-of select="- $image_size * 0.4"/></xsl:attribute>
                            <xsl:attribute name="x2"><xsl:value-of select="- $image_size * 0.0625"/></xsl:attribute>
                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', $asc, ',0,0)')" /></xsl:attribute>
                        </line>
                        <line id="ascendant" y1="0" y2="0" class="axis axis-end">
                            <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.4"/></xsl:attribute>
                            <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.0625"/></xsl:attribute>
                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', $asc, ')')" /></xsl:attribute>
                        </line>
                        <xsl:variable name="mc" select="chartinfo/ascmcs/mc/@degree_ut"/>
                        <line id="ic" y1="0" y2="0" class="axis">
                            <xsl:attribute name="x1"><xsl:value-of select="- $image_size * 0.4"/></xsl:attribute>
                            <xsl:attribute name="x2"><xsl:value-of select="- $image_size * 0.0625"/></xsl:attribute>
                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', $mc, ')')" /></xsl:attribute>
                        </line>
                        <line id="mc" y1="0" y2="0" class="axis axis-end">
                            <xsl:attribute name="x1"><xsl:value-of select="$image_size * 0.4"/></xsl:attribute>
                            <xsl:attribute name="x2"><xsl:value-of select="$image_size * 0.0625"/></xsl:attribute>
                            <xsl:attribute name="transform"><xsl:value-of select="concat('rotate(-', $mc, ')')" /></xsl:attribute>
                        </line>
                    </g>

                    <g id="planets">
                        <xsl:for-each select="chartinfo/ascmcs/vertex">
                            <xsl:call-template name="planet-template">
                                <xsl:with-param name="planet_name">vertex</xsl:with-param>
                                <xsl:with-param name="rotate"><xsl:value-of select="@degree_ut"/></xsl:with-param>
                                <xsl:with-param name="planet_base">point_vertex</xsl:with-param>
                                <!-- TODO: dist must be calculated for Vertex, too! -->
                                <xsl:with-param name="dist">0</xsl:with-param>
                                <xsl:with-param name="retrograde">False</xsl:with-param>
                            </xsl:call-template>
                        </xsl:for-each>
                        <xsl:for-each select="chartinfo/bodies/body">
                            <xsl:call-template name="planet-template">
                                <xsl:with-param name="planet_name"><xsl:value-of select="@name"/></xsl:with-param>
                                <xsl:with-param name="rotate"><xsl:value-of select="@degree"/></xsl:with-param>
                                <xsl:with-param name="planet_base">planet_<xsl:value-of select="translate(@name, '-', '_')"/></xsl:with-param>
                                <xsl:with-param name="dist"><xsl:value-of select="@dist"/></xsl:with-param>
                                <xsl:with-param name="retrograde"><xsl:value-of select="@retrograde"/></xsl:with-param>
                            </xsl:call-template>

                            <xsl:choose>
                                <xsl:when test="@name='moon-node'">
                                    <xsl:call-template name="planet-template">
                                        <xsl:with-param name="planet_name"><xsl:value-of select="@name"/>-desc</xsl:with-param>
                                        <xsl:with-param name="rotate"><xsl:value-of select="180 + @degree"/></xsl:with-param>
                                        <xsl:with-param name="planet_base">planet_moon_node</xsl:with-param>
                                        <xsl:with-param name="dist"><xsl:value-of select="@dist"/></xsl:with-param>
                                        <xsl:with-param name="retrograde"><xsl:value-of select="@retrograde"/></xsl:with-param>
                                        <xsl:with-param name="upside-down">yes</xsl:with-param>
                                    </xsl:call-template>
                                </xsl:when>
                            </xsl:choose>
                        </xsl:for-each>
                    </g>

                    <g id="aspects">
                        <xsl:for-each select="chartinfo/aspects/aspect">
                            <xsl:variable name="planet1" select="@body1"/>
                            <xsl:variable name="deg1">
                                <xsl:choose>
                                    <xsl:when test="$planet1='ascendant'">
                                        <xsl:value-of select="/chartinfo/ascmcs/ascendant/@degree_ut" />
                                    </xsl:when>
                                    <xsl:when test="$planet1='mc'">
                                        <xsl:value-of select="/chartinfo/ascmcs/mc/@degree_ut" />
                                    </xsl:when>
                                    <xsl:when test="$planet1='vertex'">
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
                                    <xsl:when test="$planet2='ascendant'">
                                        <xsl:value-of select="/chartinfo/ascmcs/ascendant/@degree_ut" />
                                    </xsl:when>
                                    <xsl:when test="$planet2='mc'">
                                        <xsl:value-of select="/chartinfo/ascmcs/mc/@degree_ut" />
                                    </xsl:when>
                                    <xsl:when test="$planet2='vertex'">
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

                            <line class="aspect">
                                <xsl:attribute name="id">aspect-<xsl:value-of select="$planet1"/>-<xsl:value-of select="$planet2"/></xsl:attribute>
                                <xsl:attribute name="class">aspect aspect-<xsl:value-of select="@type"/> aspect-<xsl:value-of select="$planet1"/> aspect-<xsl:value-of select="$planet2"/></xsl:attribute>
                                <xsl:attribute name="x1"><xsl:value-of select="$x1"/></xsl:attribute>
                                <xsl:attribute name="y1"><xsl:value-of select="$y1"/></xsl:attribute>
                                <xsl:attribute name="x2"><xsl:value-of select="$x2"/></xsl:attribute>
                                <xsl:attribute name="y2"><xsl:value-of select="$y2"/></xsl:attribute>
                            </line>
                        </xsl:for-each>
                    </g>

                    <g id="antiscia" display="none">
                        <xsl:for-each select="chartinfo/antiscia/antiscia">
                            <xsl:variable name="planet1" select="@body1"/>
                            <xsl:variable name="deg1">
                                <xsl:choose>
                                    <xsl:when test="$planet1='ascendant'">
                                        <xsl:value-of select="/chartinfo/ascmcs/ascendant/@degree_ut" />
                                    </xsl:when>
                                    <xsl:when test="$planet1='mc'">
                                        <xsl:value-of select="/chartinfo/ascmcs/mc/@degree_ut" />
                                    </xsl:when>
                                    <xsl:when test="$planet1='vertex'">
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
                                    <xsl:when test="$planet2='ascendant'">
                                        <xsl:value-of select="/chartinfo/ascmcs/ascendant/@degree_ut" />
                                    </xsl:when>
                                    <xsl:when test="$planet2='mc'">
                                        <xsl:value-of select="/chartinfo/ascmcs/mc/@degree_ut" />
                                    </xsl:when>
                                    <xsl:when test="$planet2='vertex'">
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

                            <line class="antiscion">
                                <xsl:attribute name="id">antiscion-<xsl:value-of select="$planet1"/>-<xsl:value-of select="$planet2"/></xsl:attribute>
                                <xsl:attribute name="class">antiscion antiscion-<xsl:value-of select="@axis"/> antiscion-<xsl:value-of select="$planet1"/> antiscion-<xsl:value-of select="$planet2"/></xsl:attribute>
                                <xsl:attribute name="x1"><xsl:value-of select="$x1"/></xsl:attribute>
                                <xsl:attribute name="y1"><xsl:value-of select="$y1"/></xsl:attribute>
                                <xsl:attribute name="x2"><xsl:value-of select="$x2"/></xsl:attribute>
                                <xsl:attribute name="y2"><xsl:value-of select="$y2"/></xsl:attribute>
                            </line>
                        </xsl:for-each>
                    </g>
                </g>
            </g>
        </svg>
    </xsl:template>
</xsl:stylesheet>
