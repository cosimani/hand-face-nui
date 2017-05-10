#include "speech.hpp"

Speech::Speech( QObject *parent ) :
    QObject( parent )
{

}

int Speech::SynthCallback( short *wav, int numsamples, espeak_EVENT *events )
{
    Q_UNUSED( wav );
    Q_UNUSED( numsamples );
    Q_UNUSED( events );
    return 0;
}

void Speech::read( QString text )
{
    espeak_ERROR speakErr;

    if( espeak_Initialize( AUDIO_OUTPUT_SYNCH_PLAYBACK,
                           0,
                           NULL,
                           espeakINITIALIZE_PHONEME_EVENTS ) < 0 )
    {
        // No se pudo inicializar
        return;
    }

    char textBuff[255] = { 0 };

    strcpy( textBuff, text.toStdString().c_str() );

    if( ( speakErr = espeak_Synth( textBuff,
                                   strlen(textBuff),
                                   0,
                                   POS_SENTENCE,
                                   0,
                                   espeakCHARS_AUTO,
                                   NULL,
                                   NULL ) ) != EE_OK )
    {
        // Error en la sintesis
        return;
    }
}
