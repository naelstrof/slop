/*
  File autogenerated by gengetopt version 2.22.6
  generated with the following command:
  gengetopt 

  The developers of gengetopt consider the fixed text that goes in all
  gengetopt output files to be in the public domain:
  we make no copyright claims on it.
*/

/* If we use autoconf.  */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef FIX_UNUSED
#define FIX_UNUSED(X) (void) (X) /* avoid warnings for unused params */
#endif

#include <getopt.h>

#include "cmdline.h"

const char *gengetopt_args_info_purpose = "";

const char *gengetopt_args_info_usage = "Usage: slop [options]";

const char *gengetopt_args_info_versiontext = "Copyright (C) 2014 Dalton Nell, Slop Contributors\n(https://github.com/naelstrof/slop/graphs/contributors)";

const char *gengetopt_args_info_description = "slop (Select Operation) is an application that queries for a selection from the\nuser and prints the region to stdout.";

const char *gengetopt_args_info_help[] = {
  "  -h, --help                    Print help and exit",
  "  -V, --version                 Print version and exit",
  "Options",
  "      --xdisplay=hostname:number.screen_number\n                                Sets the x display.  (default=`:0')",
  "      --nokeyboard              Disables the ability to cancel selections with\n                                  the keyboard.  (default=off)",
  "  -b, --bordersize=INT          Set the selection rectangle's thickness. Does\n                                  nothing when --highlight is enabled.\n                                  (default=`5')",
  "  -p, --padding=INT             Set the padding size of the selection. Can be\n                                  negative.  (default=`0')",
  "  -t, --tolerance=INT           How far in pixels the mouse can move after\n                                  clicking and still be detected as a normal\n                                  click instead of a click and drag. Setting\n                                  this to 0 will disable window selections.\n                                  (default=`2')",
  "  -g, --gracetime=FLOAT         Set the amount of time before slop will check\n                                  for keyboard cancellations in seconds.\n                                  (default=`0.4')",
  "  -c, --color=FLOAT,FLOAT,FLOAT,FLOAT\n                                Set the selection rectangle's color. Supports\n                                  RGB or RGBA values.\n                                  (default=`0.5,0.5,0.5,1')",
  "  -n, --nodecorations           Attempt to select child windows in order to\n                                  avoid window decorations.  (default=off)",
  "      --min=INT                 Set the minimum output of width or height\n                                  values. This is useful to avoid outputting 0.\n                                  Setting min and max to the same value\n                                  disables drag selections.  (default=`0')",
  "      --max=INT                 Set the maximum output of width or height\n                                  values. Setting min and max to the same value\n                                  disables drag selections.  (default=`0')",
  "  -l, --highlight               Instead of outlining selections, slop\n                                  highlights it. This is only useful when\n                                  --color is set to a transparent color.\n                                  (default=off)",
  "\nExamples\n    $ # Gray, thick, transparent border for maximum visiblity.\n    $ slop -b 20 -c 0.5,0.5,0.5,0.8\n\n    $ # Remove window decorations.\n    $ slop --nodecorations\n\n    $ # Disable window selections. Useful for selecting individual pixels.\n    $ slop -t 0\n\n    $ # Classic Windows XP selection.\n    $ slop -l -c 0.3,0.4,0.6,0.4\n",
    0
};

typedef enum {ARG_NO
  , ARG_FLAG
  , ARG_STRING
  , ARG_INT
} cmdline_parser_arg_type;

static
void clear_given (struct gengetopt_args_info *args_info);
static
void clear_args (struct gengetopt_args_info *args_info);

static int
cmdline_parser_internal (int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error);


static char *
gengetopt_strdup (const char *s);

static
void clear_given (struct gengetopt_args_info *args_info)
{
  args_info->help_given = 0 ;
  args_info->version_given = 0 ;
  args_info->xdisplay_given = 0 ;
  args_info->nokeyboard_given = 0 ;
  args_info->bordersize_given = 0 ;
  args_info->padding_given = 0 ;
  args_info->tolerance_given = 0 ;
  args_info->gracetime_given = 0 ;
  args_info->color_given = 0 ;
  args_info->nodecorations_given = 0 ;
  args_info->min_given = 0 ;
  args_info->max_given = 0 ;
  args_info->highlight_given = 0 ;
}

static
void clear_args (struct gengetopt_args_info *args_info)
{
  FIX_UNUSED (args_info);
  args_info->xdisplay_arg = gengetopt_strdup (":0");
  args_info->xdisplay_orig = NULL;
  args_info->nokeyboard_flag = 0;
  args_info->bordersize_arg = 5;
  args_info->bordersize_orig = NULL;
  args_info->padding_arg = 0;
  args_info->padding_orig = NULL;
  args_info->tolerance_arg = 2;
  args_info->tolerance_orig = NULL;
  args_info->gracetime_arg = gengetopt_strdup ("0.4");
  args_info->gracetime_orig = NULL;
  args_info->color_arg = gengetopt_strdup ("0.5,0.5,0.5,1");
  args_info->color_orig = NULL;
  args_info->nodecorations_flag = 0;
  args_info->min_arg = 0;
  args_info->min_orig = NULL;
  args_info->max_arg = 0;
  args_info->max_orig = NULL;
  args_info->highlight_flag = 0;
  
}

static
void init_args_info(struct gengetopt_args_info *args_info)
{


  args_info->help_help = gengetopt_args_info_help[0] ;
  args_info->version_help = gengetopt_args_info_help[1] ;
  args_info->xdisplay_help = gengetopt_args_info_help[3] ;
  args_info->nokeyboard_help = gengetopt_args_info_help[4] ;
  args_info->bordersize_help = gengetopt_args_info_help[5] ;
  args_info->padding_help = gengetopt_args_info_help[6] ;
  args_info->tolerance_help = gengetopt_args_info_help[7] ;
  args_info->gracetime_help = gengetopt_args_info_help[8] ;
  args_info->color_help = gengetopt_args_info_help[9] ;
  args_info->nodecorations_help = gengetopt_args_info_help[10] ;
  args_info->min_help = gengetopt_args_info_help[11] ;
  args_info->max_help = gengetopt_args_info_help[12] ;
  args_info->highlight_help = gengetopt_args_info_help[13] ;
  
}

void
cmdline_parser_print_version (void)
{
  printf ("%s %s\n",
     (strlen(CMDLINE_PARSER_PACKAGE_NAME) ? CMDLINE_PARSER_PACKAGE_NAME : CMDLINE_PARSER_PACKAGE),
     CMDLINE_PARSER_VERSION);

  if (strlen(gengetopt_args_info_versiontext) > 0)
    printf("\n%s\n", gengetopt_args_info_versiontext);
}

static void print_help_common(void) {
  cmdline_parser_print_version ();

  if (strlen(gengetopt_args_info_purpose) > 0)
    printf("\n%s\n", gengetopt_args_info_purpose);

  if (strlen(gengetopt_args_info_usage) > 0)
    printf("\n%s\n", gengetopt_args_info_usage);

  printf("\n");

  if (strlen(gengetopt_args_info_description) > 0)
    printf("%s\n\n", gengetopt_args_info_description);
}

void
cmdline_parser_print_help (void)
{
  int i = 0;
  print_help_common();
  while (gengetopt_args_info_help[i])
    printf("%s\n", gengetopt_args_info_help[i++]);
}

void
cmdline_parser_init (struct gengetopt_args_info *args_info)
{
  clear_given (args_info);
  clear_args (args_info);
  init_args_info (args_info);
}

void
cmdline_parser_params_init(struct cmdline_parser_params *params)
{
  if (params)
    { 
      params->override = 0;
      params->initialize = 1;
      params->check_required = 1;
      params->check_ambiguity = 0;
      params->print_errors = 1;
    }
}

struct cmdline_parser_params *
cmdline_parser_params_create(void)
{
  struct cmdline_parser_params *params = 
    (struct cmdline_parser_params *)malloc(sizeof(struct cmdline_parser_params));
  cmdline_parser_params_init(params);  
  return params;
}

static void
free_string_field (char **s)
{
  if (*s)
    {
      free (*s);
      *s = 0;
    }
}


static void
cmdline_parser_release (struct gengetopt_args_info *args_info)
{

  free_string_field (&(args_info->xdisplay_arg));
  free_string_field (&(args_info->xdisplay_orig));
  free_string_field (&(args_info->bordersize_orig));
  free_string_field (&(args_info->padding_orig));
  free_string_field (&(args_info->tolerance_orig));
  free_string_field (&(args_info->gracetime_arg));
  free_string_field (&(args_info->gracetime_orig));
  free_string_field (&(args_info->color_arg));
  free_string_field (&(args_info->color_orig));
  free_string_field (&(args_info->min_orig));
  free_string_field (&(args_info->max_orig));
  
  

  clear_given (args_info);
}


static void
write_into_file(FILE *outfile, const char *opt, const char *arg, const char *values[])
{
  FIX_UNUSED (values);
  if (arg) {
    fprintf(outfile, "%s=\"%s\"\n", opt, arg);
  } else {
    fprintf(outfile, "%s\n", opt);
  }
}


int
cmdline_parser_dump(FILE *outfile, struct gengetopt_args_info *args_info)
{
  int i = 0;

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot dump options to stream\n", CMDLINE_PARSER_PACKAGE);
      return EXIT_FAILURE;
    }

  if (args_info->help_given)
    write_into_file(outfile, "help", 0, 0 );
  if (args_info->version_given)
    write_into_file(outfile, "version", 0, 0 );
  if (args_info->xdisplay_given)
    write_into_file(outfile, "xdisplay", args_info->xdisplay_orig, 0);
  if (args_info->nokeyboard_given)
    write_into_file(outfile, "nokeyboard", 0, 0 );
  if (args_info->bordersize_given)
    write_into_file(outfile, "bordersize", args_info->bordersize_orig, 0);
  if (args_info->padding_given)
    write_into_file(outfile, "padding", args_info->padding_orig, 0);
  if (args_info->tolerance_given)
    write_into_file(outfile, "tolerance", args_info->tolerance_orig, 0);
  if (args_info->gracetime_given)
    write_into_file(outfile, "gracetime", args_info->gracetime_orig, 0);
  if (args_info->color_given)
    write_into_file(outfile, "color", args_info->color_orig, 0);
  if (args_info->nodecorations_given)
    write_into_file(outfile, "nodecorations", 0, 0 );
  if (args_info->min_given)
    write_into_file(outfile, "min", args_info->min_orig, 0);
  if (args_info->max_given)
    write_into_file(outfile, "max", args_info->max_orig, 0);
  if (args_info->highlight_given)
    write_into_file(outfile, "highlight", 0, 0 );
  

  i = EXIT_SUCCESS;
  return i;
}

int
cmdline_parser_file_save(const char *filename, struct gengetopt_args_info *args_info)
{
  FILE *outfile;
  int i = 0;

  outfile = fopen(filename, "w");

  if (!outfile)
    {
      fprintf (stderr, "%s: cannot open file for writing: %s\n", CMDLINE_PARSER_PACKAGE, filename);
      return EXIT_FAILURE;
    }

  i = cmdline_parser_dump(outfile, args_info);
  fclose (outfile);

  return i;
}

void
cmdline_parser_free (struct gengetopt_args_info *args_info)
{
  cmdline_parser_release (args_info);
}

/** @brief replacement of strdup, which is not standard */
char *
gengetopt_strdup (const char *s)
{
  char *result = 0;
  if (!s)
    return result;

  result = (char*)malloc(strlen(s) + 1);
  if (result == (char*)0)
    return (char*)0;
  strcpy(result, s);
  return result;
}

int
cmdline_parser (int argc, char **argv, struct gengetopt_args_info *args_info)
{
  return cmdline_parser2 (argc, argv, args_info, 0, 1, 1);
}

int
cmdline_parser_ext (int argc, char **argv, struct gengetopt_args_info *args_info,
                   struct cmdline_parser_params *params)
{
  int result;
  result = cmdline_parser_internal (argc, argv, args_info, params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser2 (int argc, char **argv, struct gengetopt_args_info *args_info, int override, int initialize, int check_required)
{
  int result;
  struct cmdline_parser_params params;
  
  params.override = override;
  params.initialize = initialize;
  params.check_required = check_required;
  params.check_ambiguity = 0;
  params.print_errors = 1;

  result = cmdline_parser_internal (argc, argv, args_info, &params, 0);

  if (result == EXIT_FAILURE)
    {
      cmdline_parser_free (args_info);
      exit (EXIT_FAILURE);
    }
  
  return result;
}

int
cmdline_parser_required (struct gengetopt_args_info *args_info, const char *prog_name)
{
  FIX_UNUSED (args_info);
  FIX_UNUSED (prog_name);
  return EXIT_SUCCESS;
}


static char *package_name = 0;

/**
 * @brief updates an option
 * @param field the generic pointer to the field to update
 * @param orig_field the pointer to the orig field
 * @param field_given the pointer to the number of occurrence of this option
 * @param prev_given the pointer to the number of occurrence already seen
 * @param value the argument for this option (if null no arg was specified)
 * @param possible_values the possible values for this option (if specified)
 * @param default_value the default value (in case the option only accepts fixed values)
 * @param arg_type the type of this option
 * @param check_ambiguity @see cmdline_parser_params.check_ambiguity
 * @param override @see cmdline_parser_params.override
 * @param no_free whether to free a possible previous value
 * @param multiple_option whether this is a multiple option
 * @param long_opt the corresponding long option
 * @param short_opt the corresponding short option (or '-' if none)
 * @param additional_error possible further error specification
 */
static
int update_arg(void *field, char **orig_field,
               unsigned int *field_given, unsigned int *prev_given, 
               char *value, const char *possible_values[],
               const char *default_value,
               cmdline_parser_arg_type arg_type,
               int check_ambiguity, int override,
               int no_free, int multiple_option,
               const char *long_opt, char short_opt,
               const char *additional_error)
{
  char *stop_char = 0;
  const char *val = value;
  int found;
  char **string_field;
  FIX_UNUSED (field);

  stop_char = 0;
  found = 0;

  if (!multiple_option && prev_given && (*prev_given || (check_ambiguity && *field_given)))
    {
      if (short_opt != '-')
        fprintf (stderr, "%s: `--%s' (`-%c') option given more than once%s\n", 
               package_name, long_opt, short_opt,
               (additional_error ? additional_error : ""));
      else
        fprintf (stderr, "%s: `--%s' option given more than once%s\n", 
               package_name, long_opt,
               (additional_error ? additional_error : ""));
      return 1; /* failure */
    }

  FIX_UNUSED (default_value);
    
  if (field_given && *field_given && ! override)
    return 0;
  if (prev_given)
    (*prev_given)++;
  if (field_given)
    (*field_given)++;
  if (possible_values)
    val = possible_values[found];

  switch(arg_type) {
  case ARG_FLAG:
    *((int *)field) = !*((int *)field);
    break;
  case ARG_INT:
    if (val) *((int *)field) = strtol (val, &stop_char, 0);
    break;
  case ARG_STRING:
    if (val) {
      string_field = (char **)field;
      if (!no_free && *string_field)
        free (*string_field); /* free previous string */
      *string_field = gengetopt_strdup (val);
    }
    break;
  default:
    break;
  };

  /* check numeric conversion */
  switch(arg_type) {
  case ARG_INT:
    if (val && !(stop_char && *stop_char == '\0')) {
      fprintf(stderr, "%s: invalid numeric value: %s\n", package_name, val);
      return 1; /* failure */
    }
    break;
  default:
    ;
  };

  /* store the original value */
  switch(arg_type) {
  case ARG_NO:
  case ARG_FLAG:
    break;
  default:
    if (value && orig_field) {
      if (no_free) {
        *orig_field = value;
      } else {
        if (*orig_field)
          free (*orig_field); /* free previous string */
        *orig_field = gengetopt_strdup (value);
      }
    }
  };

  return 0; /* OK */
}


int
cmdline_parser_internal (
  int argc, char **argv, struct gengetopt_args_info *args_info,
                        struct cmdline_parser_params *params, const char *additional_error)
{
  int c;	/* Character of the parsed option.  */

  int error_occurred = 0;
  struct gengetopt_args_info local_args_info;
  
  int override;
  int initialize;
  int check_required;
  int check_ambiguity;
  
  package_name = argv[0];
  
  override = params->override;
  initialize = params->initialize;
  check_required = params->check_required;
  check_ambiguity = params->check_ambiguity;

  if (initialize)
    cmdline_parser_init (args_info);

  cmdline_parser_init (&local_args_info);

  optarg = 0;
  optind = 0;
  opterr = params->print_errors;
  optopt = '?';

  while (1)
    {
      int option_index = 0;

      static struct option long_options[] = {
        { "help",	0, NULL, 'h' },
        { "version",	0, NULL, 'V' },
        { "xdisplay",	1, NULL, 0 },
        { "nokeyboard",	0, NULL, 0 },
        { "bordersize",	1, NULL, 'b' },
        { "padding",	1, NULL, 'p' },
        { "tolerance",	1, NULL, 't' },
        { "gracetime",	1, NULL, 'g' },
        { "color",	1, NULL, 'c' },
        { "nodecorations",	0, NULL, 'n' },
        { "min",	1, NULL, 0 },
        { "max",	1, NULL, 0 },
        { "highlight",	0, NULL, 'l' },
        { 0,  0, 0, 0 }
      };

      c = getopt_long (argc, argv, "hVb:p:t:g:c:nl", long_options, &option_index);

      if (c == -1) break;	/* Exit from `while (1)' loop.  */

      switch (c)
        {
        case 'h':	/* Print help and exit.  */
          cmdline_parser_print_help ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'V':	/* Print version and exit.  */
          cmdline_parser_print_version ();
          cmdline_parser_free (&local_args_info);
          exit (EXIT_SUCCESS);

        case 'b':	/* Set the selection rectangle's thickness. Does nothing when --highlight is enabled..  */
        
        
          if (update_arg( (void *)&(args_info->bordersize_arg), 
               &(args_info->bordersize_orig), &(args_info->bordersize_given),
              &(local_args_info.bordersize_given), optarg, 0, "5", ARG_INT,
              check_ambiguity, override, 0, 0,
              "bordersize", 'b',
              additional_error))
            goto failure;
        
          break;
        case 'p':	/* Set the padding size of the selection. Can be negative..  */
        
        
          if (update_arg( (void *)&(args_info->padding_arg), 
               &(args_info->padding_orig), &(args_info->padding_given),
              &(local_args_info.padding_given), optarg, 0, "0", ARG_INT,
              check_ambiguity, override, 0, 0,
              "padding", 'p',
              additional_error))
            goto failure;
        
          break;
        case 't':	/* How far in pixels the mouse can move after clicking and still be detected as a normal click instead of a click and drag. Setting this to 0 will disable window selections..  */
        
        
          if (update_arg( (void *)&(args_info->tolerance_arg), 
               &(args_info->tolerance_orig), &(args_info->tolerance_given),
              &(local_args_info.tolerance_given), optarg, 0, "2", ARG_INT,
              check_ambiguity, override, 0, 0,
              "tolerance", 't',
              additional_error))
            goto failure;
        
          break;
        case 'g':	/* Set the amount of time before slop will check for keyboard cancellations in seconds..  */
        
        
          if (update_arg( (void *)&(args_info->gracetime_arg), 
               &(args_info->gracetime_orig), &(args_info->gracetime_given),
              &(local_args_info.gracetime_given), optarg, 0, "0.4", ARG_STRING,
              check_ambiguity, override, 0, 0,
              "gracetime", 'g',
              additional_error))
            goto failure;
        
          break;
        case 'c':	/* Set the selection rectangle's color. Supports RGB or RGBA values..  */
        
        
          if (update_arg( (void *)&(args_info->color_arg), 
               &(args_info->color_orig), &(args_info->color_given),
              &(local_args_info.color_given), optarg, 0, "0.5,0.5,0.5,1", ARG_STRING,
              check_ambiguity, override, 0, 0,
              "color", 'c',
              additional_error))
            goto failure;
        
          break;
        case 'n':	/* Attempt to select child windows in order to avoid window decorations..  */
        
        
          if (update_arg((void *)&(args_info->nodecorations_flag), 0, &(args_info->nodecorations_given),
              &(local_args_info.nodecorations_given), optarg, 0, 0, ARG_FLAG,
              check_ambiguity, override, 1, 0, "nodecorations", 'n',
              additional_error))
            goto failure;
        
          break;
        case 'l':	/* Instead of outlining selections, slop highlights it. This is only useful when --color is set to a transparent color..  */
        
        
          if (update_arg((void *)&(args_info->highlight_flag), 0, &(args_info->highlight_given),
              &(local_args_info.highlight_given), optarg, 0, 0, ARG_FLAG,
              check_ambiguity, override, 1, 0, "highlight", 'l',
              additional_error))
            goto failure;
        
          break;

        case 0:	/* Long option with no short option */
          /* Sets the x display..  */
          if (strcmp (long_options[option_index].name, "xdisplay") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->xdisplay_arg), 
                 &(args_info->xdisplay_orig), &(args_info->xdisplay_given),
                &(local_args_info.xdisplay_given), optarg, 0, ":0", ARG_STRING,
                check_ambiguity, override, 0, 0,
                "xdisplay", '-',
                additional_error))
              goto failure;
          
          }
          /* Disables the ability to cancel selections with the keyboard..  */
          else if (strcmp (long_options[option_index].name, "nokeyboard") == 0)
          {
          
          
            if (update_arg((void *)&(args_info->nokeyboard_flag), 0, &(args_info->nokeyboard_given),
                &(local_args_info.nokeyboard_given), optarg, 0, 0, ARG_FLAG,
                check_ambiguity, override, 1, 0, "nokeyboard", '-',
                additional_error))
              goto failure;
          
          }
          /* Set the minimum output of width or height values. This is useful to avoid outputting 0. Setting min and max to the same value disables drag selections..  */
          else if (strcmp (long_options[option_index].name, "min") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->min_arg), 
                 &(args_info->min_orig), &(args_info->min_given),
                &(local_args_info.min_given), optarg, 0, "0", ARG_INT,
                check_ambiguity, override, 0, 0,
                "min", '-',
                additional_error))
              goto failure;
          
          }
          /* Set the maximum output of width or height values. Setting min and max to the same value disables drag selections..  */
          else if (strcmp (long_options[option_index].name, "max") == 0)
          {
          
          
            if (update_arg( (void *)&(args_info->max_arg), 
                 &(args_info->max_orig), &(args_info->max_given),
                &(local_args_info.max_given), optarg, 0, "0", ARG_INT,
                check_ambiguity, override, 0, 0,
                "max", '-',
                additional_error))
              goto failure;
          
          }
          
          break;
        case '?':	/* Invalid option.  */
          /* `getopt_long' already printed an error message.  */
          goto failure;

        default:	/* bug: option not considered.  */
          fprintf (stderr, "%s: option unknown: %c%s\n", CMDLINE_PARSER_PACKAGE, c, (additional_error ? additional_error : ""));
          abort ();
        } /* switch */
    } /* while */




  cmdline_parser_release (&local_args_info);

  if ( error_occurred )
    return (EXIT_FAILURE);

  return 0;

failure:
  
  cmdline_parser_release (&local_args_info);
  return (EXIT_FAILURE);
}
