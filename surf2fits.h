/*
 *
 * surf2fits.h - include file surf to fits converter
 *
 */

#ifndef	__surf2fits_h
#define	__surf2fits_h

#include <math.h>
#include <fitsio.h>
#include "util/xutil.h"
#include "util/file.h"
#include "util/macro.h"
#include "util/xalloc.h"
#include "util/xerror.h"

#define DEBUG 1
#define G_LITTLE_ENDIAN 1234
#define G_BYTE_ORDER G_LITTLE_ENDIAN

typedef char gchar;
typedef unsigned char guchar;
typedef short gint16;
typedef unsigned short guint16;
typedef unsigned int guint;
typedef int gint;
typedef int gint32;
typedef unsigned int guint32;
typedef float gfloat;
typedef double gdouble;
typedef int gsize;
typedef int gboolean;

typedef char *GwyDataField;
typedef void GwySIUnit;
typedef void GwySIUnit;
typedef void GError;

#define gwy_file_abandon_contents(buffer, size, mode) xfree(buffer)

/* original source: gwyddion-2.42/modules/file/surfile.c */

#define MAGIC "DIGITAL SURF"
#define MAGIC_SIZE (sizeof(MAGIC) - 1)

enum { SURF_HEADER_SIZE = 512 };

typedef enum {
    SURF_PC        = 0,
    SURF_MACINTOSH = 257
} SurfFormatType;

typedef enum {
    SURF_PROFILE         = 1,
    SURF_SURFACE         = 2,
    SURF_BINARY          = 3,
    SURF_SERIES_PROFILES = 4,
    SURF_SERIES_SURFACES = 5
} SurfObjectType;

typedef enum {
    SURF_ACQ_UNKNOWN        = 0,
    SURF_ACQ_STYLUS         = 1,
    SURF_ACQ_OPTICAL        = 2,
    SURF_ACQ_THERMOCOUPLE   = 3,
    SURF_ACQ_UNKNOWN_TOO    = 4,
    SURF_ACQ_STYLUS_SKID    = 5,
    SURF_ACQ_AFM            = 6,
    SURF_ACQ_STM            = 7,
    SURF_ACQ_VIDEO          = 8,
    SURF_ACQ_INTERFEROMETER = 9,
    SURF_ACQ_LIGHT          = 10,
} SurfAcqusitionType;

typedef enum {
    SURF_RANGE_NORMAL = 0,
    SURF_RANGE_HIGH   = 1,
} SurfRangeType;

typedef enum {
    SURF_SP_NORMAL      = 0,
    SURF_SP_SATURATIONS = 1,
} SurfSpecialPointsType;

typedef enum {
    SURF_INV_NONE = 0,
    SURF_INV_Z    = 1,
    SURF_FLIP_Z   = 2,
    SURF_FLOP_Z   = 3,
} SurfInversionType;

typedef enum {
    SURF_LEVELING_NONE = 0,
    SURF_LEVELING_LSM  = 1,
    SURF_LEVELING_MZ   = 2,
} SurfLevelingType;

typedef struct {
    SurfFormatType format;
    gint nobjects;
    gint version;
    SurfObjectType type;
    gchar object_name[30];
    gchar operator_name[30];
    gint material_code;
    SurfAcqusitionType acquisition;
    SurfRangeType range;
    SurfSpecialPointsType special_points;
    gboolean absolute;
    gint pointsize;
    gint zmin;
    gint zmax;
    gint xres; /* number of points per line */
    gint yres; /* number of lines */
    guint nofpoints;
    gdouble dx;
    gdouble dy;
    gdouble dz;
    gchar xaxis[16];
    gchar yaxis[16];
    gchar zaxis[16];
    gchar dx_unit[16];
    gchar dy_unit[16];
    gchar dz_unit[16];
    gchar xlength_unit[16];
    gchar ylength_unit[16];
    gchar zlength_unit[16];
    gdouble xunit_ratio;
    gdouble yunit_ratio;
    gdouble zunit_ratio;
    gint imprint;
    SurfInversionType inversion;
    SurfLevelingType leveling;
    gint seconds;
    gint minutes;
    gint hours;
    gint day;
    gint month;
    gint year;
    gint dayof;
    gfloat measurement_duration;
    gint comment_size;
    gint private_size;
    gchar client_zone[128];
    gdouble XOffset;
    gdouble YOffset;
    gdouble ZOffset;
    GwyDataField *dfield;
    GwySIUnit *xyunit;
    GwySIUnit *zunit;
    // added by EGM for surf2fits
    char *ifile;
    char *image;
    double bscale, bzero;
} SurfFile;

/* original source: gwyddion-2.42/modules/file/get.h */

static inline void
get_CHARS(gchar *dest, const guchar **p, guint size)
{
    memcpy(dest, *p, size);
    *p += size;
}

static inline void
get_CHARS0(gchar *dest, const guchar **p, guint size)
{
    memcpy(dest, *p, size);
    *p += size;
    dest[size-1] = '\0';
}

#define get_CHARARRAY(dest, p) get_CHARS(dest, p, sizeof(dest))
#define get_CHARARRAY0(dest, p) get_CHARS0(dest, p, sizeof(dest))

/* original source: gwyddion-2.42/app/gwymoduleutils-file.h */

static inline gfloat
gwy_get_gfloat_le(const guchar **p)
{
    union { guchar pp[4]; gfloat f; } z;

#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
    memcpy(z.pp, *p, sizeof(gfloat));
#else
    z.pp[0] = (*p)[3];
    z.pp[1] = (*p)[2];
    z.pp[2] = (*p)[1];
    z.pp[3] = (*p)[0];
#endif
    *p += sizeof(gfloat);
    return z.f;
}

static inline gint32
gwy_get_gint32_le(const guchar **p)
{
    union { guchar pp[4]; gint32 i; } z;

#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
    memcpy(z.pp, *p, sizeof(gint32));
#else
    z.pp[0] = (*p)[3];
    z.pp[1] = (*p)[2];
    z.pp[2] = (*p)[1];
    z.pp[3] = (*p)[0];
#endif
    *p += sizeof(gint32);
    return z.i;
}

static inline guint16
gwy_get_guint16_le(const guchar **p)
{
    union { guchar pp[2]; gint16 i; } z;

#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
    memcpy(z.pp, *p, sizeof(gint16));
#else
    z.pp[0] = (*p)[1];
    z.pp[1] = (*p)[0];
#endif
    *p += sizeof(gint16);
    return z.i;
}

static inline guint32
gwy_get_guint32_le(const guchar **p)
{
    union { guchar pp[4]; guint32 i; } z;

#if (G_BYTE_ORDER == G_LITTLE_ENDIAN)
    memcpy(z.pp, *p, sizeof(gint32));
#else
    z.pp[0] = (*p)[3];
    z.pp[1] = (*p)[2];
    z.pp[2] = (*p)[1];
    z.pp[3] = (*p)[0];
#endif
    *p += sizeof(guint32);
    return z.i;
}

/* original source: ./libgwyddion/gwymacros.h */
#if DEBUG
#    define gwy_debug(format...) fprintf(stderr, format); fprintf(stderr, "\n");
#  else
#    define gwy_debug(format...) /* */
#  endif

#endif
