
#include <gmp.h>
extern "C" {
#include <pbc.h>
}
#include <iostream>

int main(int argc, char* argv[]) {
  struct pairing_s pairing;
  struct pbc_param_s params;
  element_t Z;

  pbc_param_init_a_gen(&params, 160, 512);
  pairing_init_pbc_param(&pairing, &params);

  element_init_GT(Z, &pairing);

  std::cout << element_length_in_bytes(Z) << std::endl;
  return 0;
}