#include "presto.h"
#include "mask.h"
#include "spigot.h"
#include "fitsfile.h"
#include "fitshead.h"

extern void get_calibrated_lags(void *rawlags, float *calibrated_lags);

int main(int argc, char *argv[])
{
  FILE *infile, *outfile;
  int filenum, ii=0, offset=0;
  char outfilenm[200], rawlags[4096];
  float lags[2048];
  SPIGOT_INFO spigot;
  
  if (argc==1){
    fprintf(stderr, "Usage: dump_spigot_zerolags SPIGOT_files\n");
    exit(0);
  }
  strncpy(outfilenm, argv[1], strlen(argv[1])-10);
  strncpy(outfilenm+strlen(argv[1])-10, ".zerolags", 10);
  outfile = chkfopen(outfilenm, "wb");

  for (filenum=1; filenum<argc; filenum++){
    fprintf(stderr, "Reading from file '%s'...\n", argv[filenum]);
    infile = chkfopen(argv[filenum], "r");
    if (read_SPIGOT_header(argv[filenum], &spigot)){

      /* Upper or lower sideband? */
      //if (spigot.upper_sideband){
      //offset = spigot.lags_per_sample-1;
      //fprintf(stderr, "Using lag #%d\n", offset);
      //}

      /* Position the file stream at the beginning of the data */
      chkfseek(infile, spigot.header_len, SEEK_SET);
      
      fprintf(stderr, "%d  %d\n", spigot.lags_per_sample*spigot.bits_per_lag/8, 
	      spigot.lags_per_sample);
      
      /* Loop over the samples in the file */
      while (chkfread(rawlags, sizeof(char), 
		      spigot.lags_per_sample*spigot.bits_per_lag/8, 
		      infile)){
	get_calibrated_lags(rawlags, lags);
	ii++;
	/* fprintf(stderr, "%f\n", lags[offset]); */
	fwrite(lags+offset, sizeof(float), 1, outfile);
      }
    }
    fclose(infile);
  }
  fclose(outfile);
  fprintf(stderr, "Wrote %d zerolags to '%s'.\n", ii, outfilenm);
  return 0;
}