#include "sk_error.h"

static const char * error_descriptions[] =
{
    "No se ha producido ningun error.",
    "Los argumentos son invalidos.",
    "SK_VBE ya esta instalado.",
    "SK_VBE no esta instalado.",
    "No se ha encontrado ningun driver VESA VBE 2.0.",
    "No se ha encontrado ningun modo grafico compatible con SK.",
    "Se ha producido un error al intentar obtener informacion del diver de video.",
    "La tarjeta soporta demaisados modos de viedo.",
    "El modo de video usado por la aplicacion ya esta en uso.",
    "Error instalando el handler para la interrupcion multiplex",
    "Se ha producido un error desconocido.",
};

const char * sk_error_description(sk_error_t error)
{
    return error_descriptions[error];
}
