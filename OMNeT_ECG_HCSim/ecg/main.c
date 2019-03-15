#include "ecg.h"
#include "offload.h"

int main(){
  //  ECG_init();
  
  // ECG_execute(".");
  //edge_process_dump_file();
  edge_process();
  /*
  double a[10] = {0.122, 0.132, 0.142, 0.152, 0.162, 0.172, 0.182, 0.192, 0.1102, 0.1113};
  write_double_array(a, 10, "Haha.log");
  printf("File size is %d\n", file_size_in_bytes("Haha.log"));

 
  int size = file_size_in_bytes("Haha.log");
  char* data = read_char_array(size, "Haha.log");

  write_char_array(data, size, "Hoho.log");
  double* b=read_double_array(size, "Hoho.log");
  printf("File size is %d\n", size);

  for(int i = 0; i < 10; i++){
     printf("Value of a[] is: %f\n", a[i]);
  }
  */
  return 0;
}
