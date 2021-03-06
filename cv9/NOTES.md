### SPH - smoothed particle hydrodynamics
- hydrodynamika řešená pomocí částic
- primárně vyřešeno pro astrofyziku
  - n body problem pro velké množsví částic
- částicový systém
  - částice představuje nějakou hmotnost, případně nějaké další vlastnosti
  - nad částicemi se to nějakým kernelem interpoluje a tím zjistíme, jak se daná vlastnost chová
- diskretizuje se hmotnost, ne prostor
  - Lagrangiovské metody
    - oproti eulerovským metodám je jednodušší "zhustit" počítání
      - například v některých místech chceme husší mřížku "na hranách"
      - u sph, tam kde je větší hustota, tam je více částic
  - jednotliví participanti se k výsledku přenásobí nějakou vahou
- v každé částici je
  - hmotnost
  - pozice
  - rychlost
  - z toho můžeme dále odvodit sílu, hustotu, tlak, barvu
- počítání hustoty látky v prostoru
  - bodíky jsou náhodně rozesety v prostoru
  - suma hmotnosti částic * nějaká váha určující jaký vliv částice má
- obecný vzorec pro obecnou interpolaci jakékoliv vlastnosti
  - slajd 7 - fundamentální vzorec
    - za A si můžeme představit cokoliv, co bychom chtěli na té částici sledovat
    - pro spočítání gradientů a laplaciánů vlastnosti nám stačí spočítat gradient-laplacián V(1. a 2. derivace)toho W
      - je to polynom, takže ezy
- Navier-Stokesova rovnice
  - zrychlení = parciiální derivace podle času
    - aP - zrychlení spočítané z tlaku
    - aV - zrychlení spočítané z rychlosti
    - AG - gravitační zrychlení
    - aE - nějaká externí síla
      - Fabián nepoužil
  - spočítáme výslednou sílu působící na částici
- chceme na základě mračna bodů dostat izoplochu hladiny
  - dostaneme výsledek simulace - nemusíme simulovat sph
  - nepřímá vizualizace objemových dat
   - iso surface reconstruction
   - chceme dostat trojúhelníkovou síť
   - borec to udělal ve VTK
 - Marching Cubes
  - najdeme izoplochu
  - mám rozdělenoý prostor do mřížky a v každém bodě té mřížky si vypočítám hustotu
    - dále pracujeme s hodnotami v té mřížce
  - jakmile je hodnota
    - pod nějakým thresholdem -> 0
    - nad nějakým thresholdem -> 1
    - buňku mohu popsat pomocí osmi bitů - 2^8 možných vektorů
  -krychle má dvanáct hran
    - potřebuju vědět, které hrany mi ta rovina rozsekne
    - osmibitové číslo můžeme chápat jako index do pole
      - máme brutální tabulku o velikosti 256 řádků, kde mám předpočítáno, mezi kterými hranami je je hrana
        - v tabulce je uloženo na kterých hranách buňky leží vrcholy toho troúhelníka
        - nevím, kde přesně na té hraně budou
  - zjištění, kde přesně to je
    - interpoluju iso hodnotu
    - f_iso ten můj threshod
    - i, j - číslo rohu mé buňky
  - rozlišení mřížky získáme experimentálně
  - dataset
    - jednotlivé částice a jejich hmotnost, rychlost, pozice
    - každý soubor je jeden frame
      - je to binárka, která je popsána v readme
      - pole bytů bez paddingu, prostě načteš, vkopíruješ do struktury a čau
      velikost vzorkování - 80 krychlí -> 81 bodů na osu
