

#include "sjson.h"

// parse
// input: text buffer
// 1. read next character
// 2. if '"' and there its not open, start new string; Otherwise close it
// 3. if '{', open a new object
// 4. if '}', close the previous object
// 5. if '[', open a new array
// 6. if ']', close the previous array
// 7. if ':', read the value for the previous name
// The structure is a tree
// How you read the structure. What kind of structure it is?
// json toimii dynaamisissa kielissä paremmin, koska se samalla määrittelee 
// tietorakenteen jota ohjlma voi käyttää. Sen sijaan C:ssä tietorakenne,
// esimerkiksi tietue iin märiteltävä etkäteen ja tämän jälken joudutaan 
// kuvaamaan json esitys tietueille. Kömpelöä. Onko jopa liian kömpelöä 
// eli pitäisikö miettiä tietue pohjaista ratkaisua.
// Mutta olisiko ihanne lukea suoraan tietueita? Silloin ei tarvitsisi
// toteuttaa json-parseria, jonka perusongeelma on, että se joudutaan
// kuvaamaan tietueiksi.
//
// Huomattavasti järkevämpää on käyttää kirjastofunkitioita 



