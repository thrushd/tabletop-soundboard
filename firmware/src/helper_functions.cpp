#include "helper_functions.h"

// print out an error message and halt execution
void fatal(const char *msg, const char *msg1) {
  Serial.println();
  Serial.print("ERROR: ");
  Serial.print(msg);
  Serial.println(msg1);
  while (1);
}