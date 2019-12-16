Kode eksempel
Brede Johnsgård

Webapp i azure - TickTackToe

Dette var en test av flere teknologier som var veldig nytt til meg da det ble laget, så koden og selve produktet er holdt simpel. Det er delt i 2; en frontend side i NodeJs og React som da ble hostet som en statisk webside i azure og koblet opp til DevOps pipelines, og en backend med Nodejs og Typescript, med Socket.IO som hånterer kommunikasjon og tillater spillere å spille sammen over nett, også satt opp i azure. Jeg har fjernet noen filer og connectionstrings, men alt bortsett fra databasen burde funke lokalt

Game Engine med OpenGL (C++)

Dette var et større prosjekt hvor målet var å kunne implementere og bygge en forståelse av diverse systemer i en spillmotor.Jeg vil fremheve noen av de systemene jeg implementerte ettersom en spillmotor er veldig komplex og fokuset var ikke helhetligheten av spillmotoren, men de individuelle systemene.

Camera - Dette er en av de mest fundementale systemet som er ansvarlig for å prosjektere 3D space over til skjermen. Det skjer via ViewMatrix som tar oss fra world space til camera space (Dvs. den bruker posisjon og rotasjon til camera). Så har vi projection matrix (her kalt mPerspectiveMatrix) som tar oss fra camera space til screen space. Frustum er en visualiering av området i 3D space som kamera kan se, som også ble brukt til frustumculling for å kunne ignorere rendering av objecter som ikke er synelig.

Kollisjon mellom sphære og en ujevn flate - Dette var brukt som en test av kollisjon, men ikke generalisert til et system som kan brukes til noe annet.

Object loader - Dette systemet er ansvarlig for å lese og formatere 3D objekt data som brukes til å visualisere objektene. Den kan lese OBJ formaten som alle store modelleringsprogram kan eksportere og SOSI som er en format for kartdata hvor jeg brukte høydepunktene for å visualisere terreng av norske kommuner.

Bezier Curve - En egen implementering av bezier kurver som her genereres tilfeldig og med valgfri mengde sammenhengende kurver.
