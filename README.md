Hasičské auto ovládané doskou Arduino. Komponenty:

Plastový rám vytlačený na 3D tlačiarni
4 motory, 6V 100RPM
2 ovládače motora L298N
Vodná pumpa 3-6V 
Relé 5V
2 digitálne IR senzory ohňa
Ultrazvukový HC senzor vzdialenosti
2 modré LED 
Piezzo reproduktor
Krokový motor 5V s ovládacou doskou
5xAA batériový box

Ideálny scenár:
Po zapnúti sa autíčko otočí na mieste o 360 stupňov a hľadá oheň pomocou IR senzorov, ak ho nájde zastaví a hýbe sa smerum ku zdroju ohňa (sviečka), pričom prebiehajúcu akciu signalizujú LEDky blikaním a reproduktor sirénou, ak ho nenájde ukončí program. Po priblížení sa na cca 15cm od zdroja ohňa, merané pomocou HC senzora, zastaví a aktivuje vodnú pumpu cez relé a pohybuje tryskou umiestnenou na krokovom motore na streche auta zo strany na stranu, až kým oheň neuhasí. Potom tento proces opakuje.
