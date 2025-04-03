#include "TokyoDrift.h"
#include "pitches.h"  // Asegúrate de tener este archivo en el mismo directorio

// Definición de la melodía: arreglo con las notas a reproducir
int melody[] = {
    NOTE_E5, NOTE_G5, NOTE_A5, NOTE_E5, NOTE_G5, NOTE_A5,
    NOTE_B5, NOTE_A5, NOTE_G5, NOTE_E5
  };

// Duración de cada nota: 4 = negra, 8 = corchea, etc.
int noteDurations[] = {
    4, 8, 8, 4, 8, 8,
    4, 4, 4, 4
  };

TokyoDrift::TokyoDrift(uint8_t buzzerPin) {
    _buzzerPin = buzzerPin;
    pinMode(_buzzerPin, OUTPUT);
}

void TokyoDrift::play() {
    int numNotes = sizeof(melody) / sizeof(melody[0]);
    for (int i = 0; i < numNotes; i++) {
        // Calculamos la duración de la nota en milisegundos
        int noteDuration = 1000 / noteDurations[i];
        tone(_buzzerPin, melody[i], noteDuration);

        // Pausa entre notas (se incrementa un 30% respecto a la duración de la nota)
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        noTone(_buzzerPin);
    }
}
