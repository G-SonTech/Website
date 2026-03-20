# G-Son Allservice AB

Ett produktionsnära Flask-projekt för ett lokalt svenskt allserviceföretag i Uddevalla med omnejd.

## Funktioner

- Flask med tydlig separation mellan konfiguration, data, vyer och formulärlogik
- Startsida, om oss, tjänster, tjänstedetaljer, referenser, recensioner, offert, kontakt, FAQ, integritet, tack-sida och 404
- Kontaktformulär och offertformulär med server-side validering, flash-meddelanden och lokal lagring till CSV
- SEO-grund med unika titlar, meta descriptions, canonical-länk och JSON-LD för `LocalBusiness`
- Responsiv design med sticky header, mobilmeny, CTA-sektioner och FAQ-accordion
- Datamoduler för tjänster, referenser, recensioner, FAQ och orter för enklare vidareutveckling

## Projektstruktur

```text
.
|-- app.py
|-- config.py
|-- requirements.txt
|-- .env.example
|-- data/
|   |-- faq.py
|   |-- locations.py
|   |-- references.py
|   |-- reviews.py
|   `-- services.py
|-- instance/
|-- static/
|   |-- css/
|   |-- img/
|   `-- js/
|-- templates/
|   |-- partials/
|   `-- ...
`-- utils/
    |-- forms.py
    `-- helpers.py
```

## Installation

1. Skapa ett virtuellt environment:

```bash
python -m venv .venv
```

2. Aktivera miljön:

```bash
source .venv/bin/activate
```

På Windows PowerShell:

```powershell
.venv\Scripts\Activate.ps1
```

3. Installera dependencies:

```bash
pip install -r requirements.txt
```

4. Skapa lokal miljöfil:

```bash
cp .env.example .env
```

5. Starta appen lokalt:

```bash
python -m flask --app app run --debug
```

## .env

Följande värden kan sättas i `.env`:

- `SECRET_KEY`
- `CONTACT_EMAIL`
- `ORG_NUMBER`

## Formulärflöden

- Kontaktformulär sparas lokalt till `instance/contact_submissions.csv`
- Offertformulär sparas lokalt till `instance/quote_submissions.csv`

Strukturen är förberedd för att senare kunna kopplas till e-post, CRM, databas eller adminpanel.

## Framtida utveckling

- Admininloggning och hantering av tjänster, referenser och recensioner
- Databas i stället för lokal CSV-lagring
- Blogg eller nyhetssektion
- Riktiga bilder, referenscase och externa recensioner
- Kartintegration och fler lokala landningssidor
