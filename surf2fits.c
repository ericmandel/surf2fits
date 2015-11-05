/*
 *	Copyright (c) 2015 Smithsonian Astrophysical Observatory
 */

#include "surf2fits.h"

extern char *optarg;
extern int optind;

/* original source: gwyddion-2.42/modules/file/surfile.c */
SurfFile *readSurf(char *filename){
    int bytes;
    SurfFile surffile;
    SurfFile *surf;
    // gint coef = 0;
    guchar *buffer = NULL;
    const guchar *p;
    // GError *err = NULL;
    gsize expected_size, size = 0;
    gchar signature[12];

    gint add = 0;

    // if (!gwy_file_get_contents(filename, &buffer, &size, &err)) {
    if( !(buffer = (guchar *)FileContents(filename, 0, &size)) ){
        // err_GET_FILE_CONTENTS(error, &err);
        xerror(stderr, "can't get file contents of %s\n", filename);
        return NULL;
    }

    if (size < SURF_HEADER_SIZE + 2) {
        // err_TOO_SHORT(error);
        xerror(stderr, "input file %s is too short\n", filename);
        gwy_file_abandon_contents(buffer, size, NULL);
        return NULL;
    }

    p = buffer;

    get_CHARARRAY(signature, &p);
    if (strncmp(signature, "DIGITAL SURF", 12) != 0) {
        // err_FILE_TYPE(error, "Surf");
        xerror(stderr, "surf signature is missing from file %s\n", filename);
        gwy_file_abandon_contents(buffer, size, NULL);
        return NULL;
    }

    surffile.format = gwy_get_guint16_le(&p);
    surffile.nobjects = gwy_get_guint16_le(&p);
    surffile.version = gwy_get_guint16_le(&p);
    surffile.type = gwy_get_guint16_le(&p);
    get_CHARS0(surffile.object_name, &p, 30);
    get_CHARS0(surffile.operator_name, &p, 30);
    surffile.material_code = gwy_get_guint16_le(&p);
    surffile.acquisition = gwy_get_guint16_le(&p);
    surffile.range = gwy_get_guint16_le(&p);
    surffile.special_points = gwy_get_guint16_le(&p);
    surffile.absolute = gwy_get_guint16_le(&p);
    /*reserved*/
    p += 8;
    surffile.pointsize = gwy_get_guint16_le(&p);
    surffile.zmin = gwy_get_gint32_le(&p);
    surffile.zmax = gwy_get_gint32_le(&p);
    surffile.xres = gwy_get_gint32_le(&p);
    surffile.yres = gwy_get_gint32_le(&p);
    surffile.nofpoints = gwy_get_guint32_le(&p);

    surffile.dx = gwy_get_gfloat_le(&p);
    surffile.dy = gwy_get_gfloat_le(&p);
    surffile.dz = gwy_get_gfloat_le(&p);
    get_CHARS0(surffile.xaxis, &p, 16);
    get_CHARS0(surffile.yaxis, &p, 16);
    get_CHARS0(surffile.zaxis, &p, 16);
    get_CHARS0(surffile.dx_unit, &p, 16);
    get_CHARS0(surffile.dy_unit, &p, 16);
    get_CHARS0(surffile.dz_unit, &p, 16);
    get_CHARS0(surffile.xlength_unit, &p, 16);
    get_CHARS0(surffile.ylength_unit, &p, 16);
    get_CHARS0(surffile.zlength_unit, &p, 16);

    surffile.xunit_ratio = gwy_get_gfloat_le(&p);
    surffile.yunit_ratio = gwy_get_gfloat_le(&p);
    surffile.zunit_ratio = gwy_get_gfloat_le(&p);
    surffile.imprint = gwy_get_guint16_le(&p);
    surffile.inversion = gwy_get_guint16_le(&p);
    surffile.leveling = gwy_get_guint16_le(&p);

    p += 12;

    surffile.seconds = gwy_get_guint16_le(&p);
    surffile.minutes = gwy_get_guint16_le(&p);
    surffile.hours = gwy_get_guint16_le(&p);
    surffile.day = gwy_get_guint16_le(&p);
    surffile.month = gwy_get_guint16_le(&p);
    surffile.year = gwy_get_guint16_le(&p);
    surffile.dayof = gwy_get_guint16_le(&p);
    surffile.measurement_duration = gwy_get_gfloat_le(&p);
    p += 10;

    surffile.comment_size = gwy_get_guint16_le(&p);
    surffile.private_size = gwy_get_guint16_le(&p);

    get_CHARARRAY(surffile.client_zone, &p);
    surffile.XOffset = gwy_get_gfloat_le(&p);
    surffile.YOffset = gwy_get_gfloat_le(&p);
    surffile.ZOffset = gwy_get_gfloat_le(&p);


    gwy_debug("fileformat: %d,  n_of_objects: %d, "
              "version: %d, object_type: %d",
              surffile.format, surffile.nobjects,
              surffile.version, surffile.type);
    gwy_debug("object name: <%s>", surffile.object_name);
    gwy_debug("operator name: <%s>", surffile.operator_name);

    gwy_debug("material code: %d, acquisition type: %d",
              surffile.material_code, surffile.acquisition);
    gwy_debug("range type: %d, special points: %d, absolute: %d",
              surffile.range,
              surffile.special_points, (gint)surffile.absolute);
    gwy_debug("data point size: %d", surffile.pointsize);
    gwy_debug("zmin: %d, zmax: %d", surffile.zmin, surffile.zmax);
    gwy_debug("xres: %d, yres: %d (xres*yres = %d)",
              surffile.xres, surffile.yres, (surffile.xres*surffile.yres));
    gwy_debug("total number of points: %d", surffile.nofpoints);
    gwy_debug("dx: %g, dy: %g, dz: %g",
              surffile.dx, surffile.dy, surffile.dz);
    gwy_debug("X axis name: %16s", surffile.xaxis);
    gwy_debug("Y axis name: %16s", surffile.yaxis);
    gwy_debug("Z axis name: %16s", surffile.zaxis);
    gwy_debug("dx unit: %16s", surffile.dx_unit);
    gwy_debug("dy unit: %16s", surffile.dy_unit);
    gwy_debug("dz unit: %16s", surffile.dz_unit);
    gwy_debug("X axis unit: %16s", surffile.xlength_unit);
    gwy_debug("Y axis unit: %16s", surffile.ylength_unit);
    gwy_debug("Z axis unit: %16s", surffile.zlength_unit);
    gwy_debug("xunit_ratio: %g, yunit_ratio: %g, zunit_ratio: %g",
              surffile.xunit_ratio, surffile.yunit_ratio, surffile.zunit_ratio);
    gwy_debug("imprint: %d, inversion: %d, leveling: %d",
              surffile.imprint, surffile.inversion, surffile.leveling);
    gwy_debug("Time: %d:%d:%d, Date: %d.%d.%d",
              surffile.hours, surffile.minutes, surffile.seconds,
              surffile.day, surffile.month, surffile.year);
    gwy_debug("private zone size: %d, comment size %d",
              surffile.private_size, surffile.comment_size);

    // if (err_DIMENSION(error, surffile.xres)
    //        || err_DIMENSION(error, surffile.yres)) {
    // gwy_file_abandon_contents(buffer, size, NULL);
    //        return NULL;
    //    }
    if( surffile.xres <= 0 || surffile.yres <= 0 ){
      xerror(stderr, "invalid xres (%f) or yres (%f)\n",
	     surffile.xres, surffile.yres);
    }

    expected_size = (SURF_HEADER_SIZE
                     + surffile.pointsize/8*surffile.xres*surffile.yres);
    if (expected_size != size) {
        gwy_debug("size mismatch: expect: %d, got: %d", expected_size, size);
        if (size > expected_size){
            add = size - expected_size; /*TODO  correct this !*/
	    gwy_debug("OK ... allow %d extra bytes in header", add);
        } else {
            // err_SIZE_MISMATCH(error, expected_size, size, TRUE);
            xerror(stderr, "illegal size mismatch\n");
            gwy_file_abandon_contents(buffer, size, NULL);
	    return NULL;
        }
    }

    /* Use negated positive conditions to catch NaNs */
    if (!((surffile.dx = fabs(surffile.dx)) > 0)) {
        // g_warning("Real x step is 0.0, fixing to 1.0");
        xwarning(stderr, "Real x step is 0.0, fixing to 1.0");
        surffile.dx = 1.0;
    }
    if (!((surffile.dy = fabs(surffile.dy)) > 0)) {
        //g_warning("Real y step is 0.0, fixing to 1.0");
        xwarning(stderr, "Real y step is 0.0, fixing to 1.0");
        surffile.dy = 1.0;
    }

    p = buffer + SURF_HEADER_SIZE + add;

    // return a malloc'ed copy of the record
    surf = (SurfFile *)xmalloc(sizeof(SurfFile));
    memcpy(surf, &surffile, sizeof(SurfFile));

    // save input filename
    surf->ifile = xstrdup(filename);
    // allocate space for the image data
    bytes = surffile.pointsize / 8 * surffile.xres * surffile.yres;
    surf->image = xmalloc(bytes);
    // copy the image data
    memcpy(surf->image, p, bytes);
    // free the original file data
    xfree(buffer);
    // return the good news
    return surf;
}


int writeParams(fitsfile *ofptr, SurfFile *surf){
  int status;
  // fits_write_key(ofptr, TSTRING, "SIGNATURE", surf->signature, "signature", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "IFILE", surf->ifile, "input surf file", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "FORMAT", &surf->format, "format", &status);
  status = 0;
  fits_write_key(ofptr, TUSHORT, "NOBJECTS", &surf->nobjects, "nobjects", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "VERSION", &surf->version, "version", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "TYPE", &surf->type, "type", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "OBJECT", surf->object_name, "object_name", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "OPERATOR", surf->operator_name, "operator_name", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "MATERIAL", &surf->material_code, "material_code", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "ACQUISIT", &surf->acquisition, "acquisition", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "RANGE", &surf->range, "range", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "SPECIALP", &surf->special_points, "special_points", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "ABSOLUTE", &surf->absolute, "absolute", &status);
  // fits_write_key(ofptr, TSTRING, "RESERVED", surf->reserved, "reserved", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "POINTSIZ", &surf->pointsize, "pointsize", &status);
  status = 0;
  fits_write_key(ofptr, TINT, "ZMIN", &surf->zmin, "zmin", &status);
  status = 0;
  fits_write_key(ofptr, TINT, "ZMAX", &surf->zmax, "zmax", &status);
  status = 0;
  fits_write_key(ofptr, TINT, "XRES", &surf->xres, "xres", &status);
  status = 0;
  fits_write_key(ofptr, TINT, "YRES", &surf->yres, "yres", &status);
  status = 0;
  fits_write_key(ofptr, TINT, "NOFPOINT", &surf->nofpoints, "nofpoints", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "DX", &surf->dx, "dx", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "DY", &surf->dy, "dy", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "DZ", &surf->dz, "dz", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "XAXIS", surf->xaxis, "xaxis", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "YAXIS", surf->yaxis, "yaxis", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "ZAXIS", surf->zaxis, "zaxis", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "DX_UNIT", surf->dx_unit, "dx_unit", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "DY_UNIT", surf->dy_unit, "dy_unit", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "DZ_UNIT", surf->dz_unit, "dz_unit", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "XLENGTHU", surf->xlength_unit, "xlength_unit", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "YLENGTHU", surf->ylength_unit, "ylength_unit", &status);
  status = 0;
  fits_write_key(ofptr, TSTRING, "ZLENGTHU", surf->zlength_unit, "zlength_unit", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "XUNITRAT", &surf->xunit_ratio, "xunit_ratio", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "YUNITRAT", &surf->yunit_ratio, "yunit_ratio", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "ZUNITRAT", &surf->zunit_ratio, "zunit_ratio", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "IMPRINT", &surf->imprint, "imprint", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "INVERSIO", &surf->inversion, "inversion", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "LEVELING", &surf->leveling, "leveling", &status);
  status = 0;
  // fits_write_key(ofptr, TSTRING, "OBSOLETE", surf->obsolete, "obsolete", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "SECONDS", &surf->seconds, "seconds", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "MINUTES", &surf->minutes, "minutes", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "HOURS", &surf->hours, "hours", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "DAY", &surf->day, "day", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "MONTH", &surf->month, "month", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "YEAR", &surf->year, "year", &status);
  status = 0;
  fits_write_key(ofptr, TSHORT, "DAYOF", &surf->dayof, "dayof", &status);
  status = 0;
  fits_write_key(ofptr, TFLOAT, "MEASUREM", &surf->measurement_duration, "measurement_duration", &status);
  // fits_write_key(ofptr, TSTRING, "OBSOLETE2", surf->obsolete2, "obsolete2", &status);
  // fits_write_key(ofptr, TSHORT, "COMMENT_SIZE", &surf->comment_size, "comment_size", &status);
  // fits_write_key(ofptr, TSHORT, "PRIVATE_SIZE", &surf->private_size, "private_size", &status);
  // fits_write_key(ofptr, TSTRING, "CLIENT_Z", surf->client_zone, "client_zone", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "XOFFSET", &surf->XOffset, "XOffset", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "YOFFSET", &surf->YOffset, "YOffset", &status);
  status = 0;
  fits_write_key(ofptr, TDOUBLE, "ZOFFSET", &surf->ZOffset, "ZOffset", &status);
  // fits_write_key(ofptr, TSTRING, "RESERVEDZONE", surf->reservedzone, "reservedzone", &status);
  return 0;
}

int writeFITS(char *filename, SurfFile *surf, double bscale){
  char tbuf[SZ_LINE];
  int bitpix, datatype;
  int status=0;
  int naxis=2;
  long naxes[2], fpixel[2], lpixel[2];
  fitsfile *ofptr;

  // sanity check
  if( !filename || !surf ){
    xerror(stderr, "invalid arguments to writeFITS\n");
  }
  // input data is the same as bitpix
  switch(surf->pointsize){
  case 16:
    datatype = TSHORT;
    break;
  case 32:
    datatype = TINT;
    break;
  default:
    xerror(stderr, "unknown surf pointsize: %d\n", surf->pointsize);
    break;
  }
  // FITS params from surf params
  bitpix = surf->pointsize;
  naxes[0] = surf->xres;
  naxes[1] = surf->yres;
  fpixel[0] = 1;
  fpixel[1] = 1;
  lpixel[0] = naxes[0];
  lpixel[1] = naxes[1];
  // create an empty FITS file
  fits_create_file(&ofptr, filename, &status);
  if( status ){
    fits_get_errstatus(status, tbuf);
    xerror(stderr, "ERROR: can't open output FITS file '%s' [%s]\n",
	   filename, tbuf);
    return 1;
  }
  // create a new image HDU
  fits_create_img(ofptr, bitpix, naxis, naxes, &status);
  if( status ){
    fits_get_errstatus(status, tbuf);
    xerror(stderr, "ERROR: can't create FITS image HDU '%s' [%s]\n",
	   filename, tbuf);
    return 1;
  }
  // write surf params to header
  writeParams(ofptr, surf);
  // write FITS image data to HDU
  fits_write_subset(ofptr, datatype, fpixel, lpixel, surf->image, &status);
  if( status ){
    fits_get_errstatus(status, tbuf);
    xerror(stderr, "ERROR: can't write FITS image HDU '%s' [%s]\n",
	   filename, tbuf);
    return 1;
  }
  // bscale processing has to come after image is written or else cfitsio
  // gets a data conversion error. Yikes!
  if( bscale != 0.0 ){
    surf->bscale = bscale;
    gwy_debug("bscale from command line: %f", surf->bscale);
  } else if( (surf->dz_unit[0] == -26) && (surf->dz_unit[1] == 109) ){
    surf->bscale = 1.0E-04;
    gwy_debug("bscale from header: %f", surf->bscale);
  } else {
    gwy_debug("no bscale applied (angstroms)");
  }
  if( surf->bscale != 1.0 ){
    status = 0;
    fits_write_key(ofptr, TDOUBLE, "BSCALE", &surf->bscale, "bscale", &status);
  }
  if( surf->bzero != 0.0 ){
    status = 0;
    fits_write_key(ofptr, TDOUBLE, "BZERO", &surf->bzero, "bzero", &status);
  }
  // close FITS file
  fits_close_file(ofptr, &status);
  return 0;
}

static void usage(char *fname){
  fprintf(stderr,
         "usage: %s <switches> surf_file fits_file\n",
	 fname);
  fprintf(stderr, "optional switches:\n");
  fprintf(stderr, "  -s [scale]    # bscale value for data (def: from header, maybe)\n");
}

int main(int argc, char **argv){
  int c, args;
  char *ifile, *ofile;
  SurfFile *surf;
  double bscale = 0.0;

  // exit on error
  setxerror(2);

  /* we want the args in the same order in which they arrived, and
     gnu getopt sometimes changes things without this */
  putenv("POSIXLY_CORRECT=true");
  /* process switch arguments */
  while ((c = getopt(argc, argv, "s:")) != -1){
    switch(c){
    case 's':
      bscale = atof(optarg);
      break;
    }
  }
  /* process remaining args */
  args = argc - optind;
  if( args < 2 ){
    usage(argv[0]);
    return 1;
  }
  ifile = argv[optind+0];
  ofile = argv[optind+1];
  // decode the surf file
  surf = readSurf(ifile);
  // write the FITS file
  (void)writeFITS(ofile, surf, bscale);
  // clean up
  xfree(surf->ifile);
  xfree(surf->image);
  xfree(surf);
  return 0;
}
