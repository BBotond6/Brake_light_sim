# Féklámpa rendeszer szimulációs projekt
## Általános leírás
A projektfeladatban egy gépjármű irányjelzőit és féklámpáját szimuláljuk. Az irányjelző lámpához a hallgatói
egységcsomagban két, fordított tollállású narancssárga LED (), a féklámpához egy szokványos piros
LED () áll rendelkezésre. A féklámpa működésére hatással lesz a pedálszögszenzor és az ultrahangos
távolságszenzor mérésének feldolgozása is. A pedálszögszenzorral a fékpedál normál üzemeltetését
szimuláljuk, míg az ultrahangos távolságmérés eredménye a vészfékezés szimulációjához nyújt alapot. A
projektfeladat négy fő eleme tehát a következő eszközök (alrendszerek) együttes működtetése lesz:
- Nyomógombok állapotának detektálása.
- LED-ek villogtatása.
- Távolságmérés ultrahangos szenzorral.
- Pedálszögszenzor pozíciójának meghatározása.
## Rendszerkövetelmények
1. Az egyik nyomógomb a „bal oldali” irányjelzőt kapcsolja be.
2. A másik nyomógomb a „jobb oldali” irányjelzőt kapcsolja be.
3. A nyomógombok megnyomása lefutó élet generál.
4. Az irányjelzők villogási frekvenciája a KRESZ által előírt 1 Hz – 2 Hz tartományba esik.
5. Ha az egyik irányjelző villog, akkor a kikapcsolás ugyanannak a nyomógombnak újbóli
megnyomásával történik.
6. Ha az egyik irányjelző villog, de a másik irányjelzőhöz tartozó gombot nyomjuk meg, akkor az
eddig villogó irányjelző kikapcsolódik, a másik viszont bekapcsolódik.
7. A két gomb együttes megnyomása rövid időkereten belül a vészvillogó üzemmódot aktiválja,
azaz a két irányjelző szinkronban villog.
8. A vészvillogó üzemmód csak a két gomb újbóli, együttes megnyomása következtében hagyható
el.
9. A LED-ek akkor világítanak, ha a vezérlésükhöz használt jelszint alacsony (inverz logika).
10. A gombok megnyomásánál a prellegés jelensége nem okozhat semmiféle, működésbeli
anomáliát.
11. A féklámpának három fokozata van: folytonos világítás, kisebb frekvenciával történő villogás
(~2 Hz) és nagyobb frekvenciával történő villogás (~4 Hz).
12. A féklámpa fokozatai az távolságmérés eredményétől és a pedálszögszenzor pozíciójától
függenek.
13. A pedálszögszenzor kimeneti PWM-jelének kitöltési tényezője (jelöljük w-vel) jó közelítéssel 10%
és 90% között vesz fel értéket. A féklámpa fokozatai a következő tartományokhoz tartoznak:
- 10% <= w < 20% A féklámpa nem világít.
- 20% <= w < 50% A féklámpa világít.
- 50% <= w < 70% A féklámpa a kisebb frekvenciával villog.
- 70% <= w <= 90% A féklámpa a nagyobb frekvenciával villog.
14. Az ultrahangos távolságmérés eredménye szerint a féklámpa az alábbiak szerinti üzemmódban
működik, ahol s a szenzor előtti objektum távolságát jelöli:
- s > 120 cm A féklámpa nem világít.
- 120 cm >= s > 80 cm A féklámpa világít.
- 80 cm >= s > 40 cm A féklámpa a kisebb frekvenciával villog.
- 40 cm >= s A féklámpa a nagyobb frekvenciával villog.
15. A pedálszögszenzorra és a távolságmérésre megfogalmazott feltételek közül mindig az erősebb
fékezésre vonatkozónak kell teljesülnie.
16. Távolságmérés kellő periodicitással történik (pl. 60 ms).
17. A pedálszögszenzor jelének feldolgozása kellő periodicitással történik (pl. 40 ms).
18. A pedálszögszenzor jelének feldolgozása a PWM-jel felfutó és lefutó éle közötti idő
megmérésével is történhet, de történhet RC-taggal történő szűrés (simítás) után a kondenzátor
feszültségének átlagolásával is.
19. Hardverinterfészek gyanánt az egyes szenzorok jeleihez az alábbi portokat rendeljük:

|Szenzorjel / LED-jel |I/O port az Arduino-n|
|----------------------------------|---------------------|
| Bal nyomógomb jele |2|
|Jobb nyomógomb jele |3|
|Bal oldali irányjelző LED-je |4|
|Jobb oldali irányjelző LED-je |5|
|Féklámpa LED-je |6|
|Ultrahangos szenzor TRIGGER jele |8|
|Ultrahangos szenzor ECHO jele |9|
|Fékpedálszenzor PWM-jele |11|
|Fékpedálszenzor simított jele a kondenzátorról átvezetve|A5|
