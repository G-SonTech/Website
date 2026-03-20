Skapa en komplett, professionell och modern hemsida i Python för ett allserviceföretag som heter:

G-Son Allservice AB

Företaget arbetar i Uddevalla-trakten i Sverige och erbjuder allservice-tjänster för privatpersoner, företag, fastighetsägare och bostadsrättsföreningar. Hemsidan ska kännas pålitlig, lokal, tydlig, stilren och lätt att använda både på dator och mobil.

VIKTIGT:
- Bygg projektet i Python med Flask.
- Använd Jinja2-templates.
- Använd HTML, CSS och gärna lite JavaScript där det behövs.
- Strukturen ska vara ren, professionell och lätt att vidareutveckla.
- Koden ska vara tydlig, kommenterad och organiserad i bra mappar.
- Resultatet ska vara produktionsnära.
- Designen ska vara modern, snabb, responsiv och SEO-vänlig.
- All text ska vara på svenska.
- Skapa en snygg visuell identitet även om ingen logga finns ännu.
- Gör sidan så att den går att köra lokalt direkt.

MÅL:
Skapa en hemsida som inger förtroende och hjälper företaget att få fler kunder i Uddevalla med omnejd. Sidan ska tydligt visa vad företaget gör, vilka tjänster som erbjuds, var företaget arbetar och hur man kontaktar dem.

TEKNISKA KRAV:
- Python + Flask
- Tydlig projektstruktur
- Bootstrap får användas om det hjälper, men sidan får gärna ha egen stil
- Responsiv design för mobil, surfplatta och desktop
- Kontaktformulär med validering
- Separat admin-läge behövs inte i första versionen, men projektet ska vara lätt att bygga ut senare
- Förbered struktur så att man senare kan lägga till:
  - referensprojekt
  - offertförfrågningar
  - bildgalleri
  - recensioner
  - blogg/nyheter
- Använd miljöfiler där det är relevant, till exempel för hemligheter
- Skapa requirements.txt
- Skapa README.md med instruktioner för installation och körning
- Lägg till en enkel .env.example om det behövs
- Använd url_for korrekt i templates
- Ha en tydlig base template som andra sidor ärver från

SKAPA FÖLJANDE SIDOR:
1. Startsida
2. Om oss
3. Tjänster
4. Tjänstedetaljsidor eller sektioner för flera tjänster
5. Kontakt
6. Tack-sida efter skickat formulär
7. 404-sida
8. Sekretess/integritetssida enkel version

STARTSIDAN SKA INNEHÅLLA:
- Stor hero-sektion med kraftfull rubrik
- Kort introduktion av företaget
- Tydlig CTA-knapp: “Kontakta oss” och “Begär offert”
- Kort lista över tjänster
- Varför välja G-Son Allservice AB
- Geografiskt område: Uddevalla med omnejd
- Enkel sektion med kundlöfte eller företagets styrkor
- Kontaktuppgifter i tydlig ruta
- En snygg footer med länkar och kontaktinfo

EXEMPEL PÅ BUDSKAP SOM KAN ANVÄNDAS I TEXTERNA:
- Pålitlig allservice i Uddevalla med omnejd
- Snabb hjälp, personlig service och noggrant utfört arbete
- För privatpersoner, företag och fastighetsägare
- Flexibla lösningar efter kundens behov

TJÄNSTER-SIDAN SKA VARA TYDLIG OCH SÄLJANDE:
Skapa flera tjänstekort eller sektioner. Tjänsterna får vara generella men relevanta för ett allservicebolag. Exempel:
- Fastighetsservice
- Reparationer och underhåll
- Montering och enklare byggservice
- Trädgårdsservice och utemiljö
- Bortforsling och praktisk hjälp
- Tillsyn och serviceuppdrag
- Företagsservice
- Hjälp vid mindre projekt och löpande underhåll

Varje tjänst ska ha:
- Titel
- Kort beskrivning
- Fördel för kunden
- CTA-knapp till kontakt/offert

OM OSS-SIDAN SKA:
- Presentera företaget som lokalt, seriöst och lösningsorienterat
- Ha en varm men professionell ton
- Beskriva att företaget hjälper kunder i Uddevalla-trakten
- Fokusera på service, kvalitet, tillgänglighet och ansvar
- Ha plats för framtida personalpresentation eller företagshistoria

KONTAKTSIDAN SKA INNEHÅLLA:
- Kontaktformulär med fält:
  - namn
  - telefon
  - e-post
  - ämne
  - meddelande
- Möjlighet att välja typ av ärende:
  - Fråga
  - Offertförfrågan
  - Boka arbete
- Tydlig bekräftelse efter skickat formulär
- Kontaktuppgifter i klartext
- Information om geografiskt område: Uddevalla med omnejd
- Öppettider som exempeltext eller plats för öppettider
- Förbered gärna en sektion för karta, även om en riktig karta inte implementeras fullt ut nu

FORMULÄRFUNKTION:
- Bygg formulärhantering i Flask
- Lägg in grundläggande server-side validering
- Visa tydliga felmeddelanden
- Visa lyckat skickat-meddelande
- Gör lösningen enkel att senare koppla till:
  - e-postutskick
  - databas
  - CRM
- Använd Flask flash messages snyggt i UI

DESIGNKRAV:
Designen ska kännas:
- Svensk
- lokal
- pålitlig
- modern
- stilren
- hantverksmässig men professionell

Färgförslag:
- mörk blå
- vit
- grå
- accentfärg i exempelvis varm orange eller grön
Designen ska inge trygghet och kvalitet.

DESIGNDETALJER:
- Stor och ren typografi
- Bra spacing
- Tydliga knappar
- Kort med skuggor eller subtila borders
- Sektioner med tydlig struktur
- Snygg sticky header eller tydlig toppnavigering
- Mobilmeny
- Hover-effekter
- Ikoner där det passar
- Footer med kontaktinformation, organisationsplats för framtiden och snabblänkar

NAVIGATION:
Skapa tydlig navigering med länkar till:
- Hem
- Om oss
- Tjänster
- Kontakt

HEADER:
- Företagsnamn tydligt synligt
- Enkel logotyp-text om grafisk logga saknas
- CTA-knapp i header: “Kontakta oss”

FOOTER:
- Företagsnamn
- Kort beskrivning
- Kontaktuppgifter
- Verksamhetsområde
- Länkar till sidor
- Integritetspolicy-länk
- Copyright-text

SEO-KRAV:
Gör hemsidan SEO-vänlig från början:
- Bra title-taggar för varje sida
- Meta descriptions
- Tydliga H1, H2, H3
- Semantisk HTML
- Lokal SEO-text för Uddevalla med omnejd
- Mänskligt skrivna texter, inte bara nyckelord
- Open Graph-basstruktur får gärna förberedas
- Favicon-struktur kan förberedas
- URL-struktur ska vara ren och tydlig

EXEMPEL PÅ SEO-ANPASSADE SIDTITLAR:
- G-Son Allservice AB | Allservice i Uddevalla med omnejd
- Tjänster | G-Son Allservice AB
- Kontakta oss | G-Son Allservice AB
- Om oss | G-Son Allservice AB

INNEHÅLL:
Skapa realistiska svenska texter till hela sidan. Texten ska inte kännas generisk eller översatt, utan naturlig och professionell. Undvik överdrivna marknadsföringsfraser. Skriv med förtroende, enkelhet och tydlighet.

SKAPA EXEMPELTEXTER FÖR:
- Hero-rubrik
- ingress
- tjänstebeskrivningar
- om oss-text
- kontaktsektion
- call-to-action-rutor
- footer-text

EXEMPEL PÅ TON:
- professionell
- tydlig
- hjälpsam
- lokal
- trygg
- seriös

PROJEKTSTRUKTUR:
Skapa en tydlig struktur ungefär enligt detta:

/project_root
  app.py
  requirements.txt
  README.md
  .env.example
  /templates
    base.html
    index.html
    about.html
    services.html
    contact.html
    thank_you.html
    privacy.html
    404.html
    /partials
      header.html
      footer.html
  /static
    /css
      style.css
      responsive.css
    /js
      main.js
    /img
      placeholder-hero.jpg
      service-placeholder.jpg
  /utils
    form_helpers.py
  /instance
    (kan lämnas tom eller förberedas)

APPLOGIK:
- Flask-app med routes för alla sidor
- Error handler för 404
- Kontaktformulär via POST
- Flash messages
- Separata funktioner där det förbättrar tydlighet
- Håll koden enkel men professionell

LÄGG TILL DETTA I README:
- Hur man installerar virtuellt environment
- Hur man installerar requirements
- Hur man kör projektet
- Hur man sätter FLASK_APP
- Hur man startar sidan lokalt
- Kort beskrivning av filstrukturen

SKRIV KODEN SÅ HÄR:
- Ren, tydlig och modulär
- Inga onödiga beroenden
- Bra namn på filer, funktioner och variabler
- Kommentarer där det hjälper
- Ingen slarvig dummykod
- Använd best practices för en mindre Flask-webbplats

FRAMTIDA UTVECKLING SOM SKA FÖRBEREDAS STRUKTURELLT:
- Bildgalleri för referensjobb
- Formulär för offertförfrågan med fler fält
- Enkel CMS-liknande hantering
- Recensioner från kunder
- FAQ-sektion
- Fler tjänstesidor
- Språkstöd i framtiden
- Enkel koppling till e-posttjänst

VIKTIGT OM RESULTATET:
Jag vill att du genererar ALLA filer med komplett innehåll, inte bara exempel eller pseudokod.
Skapa hela projektet färdigt så att det går att starta lokalt direkt.
Om någon bild saknas, använd tydliga placeholders.
Se till att hemsidan känns som en riktig företagshemsida för ett svenskt allservicebolag.

LÄGG OCKSÅ TILL:
- snygg hero-sektion
- sektion med ikonbaserade fördelar
- kort för tjänster
- välstrukturerad kontaktsektion
- mobilvänlig meny
- subtila animationer eller transitions
- tydliga CTA-knappar på flera ställen

UNDVIK:
- generisk AI-känsla
- överdrivet teknisk design
- för mycket färger
- rörig layout
- engelska texter
- ofärdiga filer
- halvfärdiga placeholders utan struktur

SLUTRESULTAT:
Leverera ett komplett Flask-projekt med alla filer, full HTML/CSS/JS/Python, svenska texter, modern design och fungerande kontaktformulär för G-Son Allservice AB i Uddevalla-trakten.