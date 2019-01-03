
Lauantaina 29.12.2018
-
1. Olen pohdiskellut koko joulun ajan peliprojektia Amiga 500:lle. Projekti on ollut mielessäni jo vuosien ajan ja jotain on myös tullut näiden vuosien aikana tehtyäkin. Varsinainen peli-idea on kuitenkin kypsynyt tavattoman hitaasti. Olen ajatellut, että peli on shoot'em up -tyyppinen, koska lapsena pidin niistä. Lisäksi räiskintäpeli olisi hyvä alusta teknisille taidonnäytöille niissä kun voi piirtää ruudun täyteen objekteja ilman turhia selittelyitä. Sittemmin olin kuitenkin alkanut epäröidä ideaa ja genreä ylipäätään; räiskintäpelit kun sattuvat pääsääntöisesti olevan äärimmäisen tylsiä.

Lopulta tulin siihen tulokseen, että peli on jossain määrin shoot'em up -tyyppinen, mutta siinä on vahva tower defence -tyyppinen elementti ja taistelut ovat taktisia: Ammuskelua ei tee hauskaksi päätön räiskintä, vaan nopeus ja tarkkuus. Lisäksi nautintoa tuottaa taktiset oivallukset.

Pelissä pelaaja on kaappari, jonka tehtävänä on yksinkertaisesti haalia rikkauksia ja teknologiaa. Niitä pelaaja hankkii kaappaamalla aluksia ja tukikohtia. Valtaaminen tapahtuu (yleensä) ymmärtämällä kohteen sisäinen logiikka ja siten sen heikkoudet. Ymmärtämistä voi helpottaa joko ostamalla tai hankkimalla itse tiedustelutietoa. Tietoa, kuten myös varusteita ja aseita, voi ostaa basaareissa, joita on tähtien välisessä avaruudessa. Tiedustelun voi puolestaan suorittaa aluksella tai tukikohdassa. Tällöin on varottava tulemasta nähdyksi, kuten Metal Gear Solid -pelissä. Tiedustelu sinänsä on kuin esineiden tutkiminen Impossible Missionissa tai Fortnitessä. Kiinnijäädessä pelaajalla on mahdollisuus pelastautua nopeilla reflekseillä, kuten Resident Evilissä. Mikäli tässä ei onnistu, saa kunnolla selkäänsä.

Alusten ja tukikohtien visualisaatio on värikäs, lohkomainen ja animoitu. Värien puolesta mieleeni tulee Rainbow Island -peli, mutta visualisointi tässä pelissä vakavaa ja asiallista. Niitä tukevat avaruusalukset on visualisoitu kuin Paradroid-pelissä. Kaikista kohteista on tietokortti, joka auttaa pelaajaa hahmottamaan alusten erilaisuutta. Tässä mieleeni tulee Metroid-peli.

Alusten ja tukikohtien välissä on tähtien välinen avaruus. Se on pelottava paikka, jossa liikkuu kaikenlaisia rikollisia ja outoja elämänmuotoja. Pelaajan kannattaa pääsääntöisesti välttää taisteluita niitä vastaan ja kulkea alukselta toiselle mahdollisimman pian.

2. Tänään aloitin varsinaisen toteuttamisen pelin ytimestä. Päätin tehdä linkitetyn listan ensimmäisenä, koska siitä on helppo aloittaa 15 vuoden tauon jälkeen ja sille on käyttöä monissa paikoissa esimerkiksi piirtoprosessorissa.

Yritän hahmottaa minkälaisia toiminnallisuuksia täytyy toteuttaa. Ideologia on, että ydin tarjoaa joitain yleisiä palveluita, kuten latausrutiinit, muistinhallinan ja lokituksen. Pyrkimys on välttää toteuttamasta ytimeen mitään pelispesifistä, niin että sitä voi käyttää jatkossa muidenkin pelien toteuttamiseen. Koska kuitenkin laitteen suorituskyky on hyvin rajallinen, on etsittävä kompromissia geneerisyyden ja suorituskyvyn välillä.

	1. Tapahtumaohjaus. Moduulissa toteutetaan Observer-malli ja tapahtumien käsittelyn ilman hyppykäskyjä. 
	2. Skeduleri. Tämän moduulin tehtävä on jakaa suoritusaikaa tapahtumien ja tehtävien käsittelyyn. Moduulin pitää arvioida suoritusaikaa ja tarvittaessa jättää joitain tehtäviä seuraavaan päivitysjaksoon.
	3. Loaderi. Tämä moduuli toteuttaa geneerisen plugin-rajapinnan. Erilaiset laturit hoitavat eri MIME-tyyppejä. Tärkeimpiä latureita ovat Hunk- (Object) ja obj-laturit.
	4. Skene. Tietorakenne, johon talletetaan pelioliot (eng. _Game Objects_). Peliolioita ovat oman aluksen ja vihollisten lisäksi muun muassa ammukset, tukikohdat, erilaiset way point -oliot, mittarit ja niin edelleen. Myös tekoäly ja muut ei-visuaaliset oliot voivat olla peliolioita. Tietorakenne on olennaisesti puu, jota päivitysjakson aikana läpikäydään.
	5. Törmäystarkastelu. Yksinkertainen suorakaidepohjainen tarkastelu.
	6. Näkyvyysalgoritmit. Moduuli tarjoaa line of sight -algoritmin sekä reitityksen.
	7. Muistinhallinta. Hyvin yksinkertainen ja tehokas muistinhallinta.
	8. Ohjainten ja näppäinten käsittely.
	9. Piirtoprosessori. Ohjaa ja optimoi piirtoa.
	10. Matematiikkakirjasto. Tukee trigonometrisia funktioita, splini-käyriä ja vektorilaskentaa.
	11. Tiedonsiirto sarja- ja rinnakkaisporttien kautta.
	12. Lokitus.
	13. Audioprosessori.
	14. Tilakoneiden hallinta.
	14. Tiedon pakkaaminen.
	15. Pääohjelma.

3. Tärkeä seikka on ohjelman varhainen testaus oikeassa ympäristössä. Tein jonkinlaisen esiselvityksen siitä, että sopivia Docker-kontteja on tarjolla. Tätä täytyy priorisoida, joskin haluan ensin saada aikaan jotain konkreettista.

Tiistai, 1.1.2019
-
1. Tänään on tarkoitus toteuttaa Makefile. Sitä varten täytyy laatia projektin hakemistorakenne. Luonnollisesti haluaisin kullekin ytimen toiminnalliselle kokonaisuudelle oman hakemiston ja sinne oman Makefilen. Lisäksi pitäisi olla ainakin kaksi targettia: toinen tuotannolle, toinen testaukseen.

Koska lopputuloksena on yksi ajettava tiedosto, niin hakemistorakennetta ei tarvitse kuljettaa käännöksessa. Lisäksi huomasin ja sitten muistin, että C-kielessä ei ole nimiavaruutta, joten tunnukset joutuu joka tapauksessa nimeämään yksilöllisesti huolimatta miten ne on hakemistorakenteessa sijoiteltu.

Aloitan tekemällä yksinkertaisen Makefilen ja katson myöhemmin, tarvitsenko jotain parempaa. Kopioin tiedoston [osoitteesta](https://www.cs.swarthmore.edu/~newhall/unixhelp/howto_makefiles.html#adv). Sitä varten asensin `makedepend` ohjelman.

Torstai, 3.1.2019
-