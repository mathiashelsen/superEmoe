#include "lda_vfct.hpp"

/*
 * This is a unit test to see if LDA/STA is working as intended.
 * For this test to run successfull, it required the correct 
 * lda_test.o object code. 
 */
void lda_runtest(void)
{
  int errorCount = 0;
  int OSR        = 3;

  fifo<unsigned char *> videoStream;
  render_X11 videoOutput = render_X11( &videoStream, OSR);
  std::thread videoThread( &render_X11::run, &videoOutput );

  Computer *computer = new Computer(&videoStream, OSR, "software/lda_test.o");

  /*
   * Test for LDA,Imm -> Negative number
   */
  computer->run(1);
  statusType status = computer->cpu->getStatus();
  if((uint8_t) computer->cpu->getAcc() != 0xef)
  {
    errorCount++;
    printf("Acc = 0x%02X, expected 0xEF\n", (uint8_t) computer->cpu->getAcc());
  }
  if( status.N == 0 || status.Z == 1)
  {
    errorCount++;
    printf("Incorrectly updated N/Z flags: %d/%d, expected 1/0\n", status.N, status.Z);
  }


  /*
   * Test for LDA,Imm -> Positive number
   */
  computer->run(1);
  status = computer->cpu->getStatus();
  if((uint8_t) computer->cpu->getAcc() != 0x0f)
  {
    errorCount++;
    printf("Acc = 0x%02X, expected 0x0F\n", (uint8_t) computer->cpu->getAcc());
  }
  if( status.N == 1 || status.Z == 1)
  {
    errorCount++;
    printf("Incorrectly updated N/Z flags: %d/%d, expected 0/0\n", status.N, status.Z);
  }


  /*
   * Test for LDA,Imm -> 0
   */
  computer->run(1);
  status = computer->cpu->getStatus();
  if((uint8_t) computer->cpu->getAcc() != 0x00)
  {
    errorCount++;
    printf("Acc = 0x%02X, expected 0x00\n", (uint8_t) computer->cpu->getAcc());
  }
  if( status.N == 1 || status.Z == 0)
  {
    errorCount++;
    printf("Incorrectly updated N/Z flags: %d/%d, expected 0/1\n", status.N, status.Z);
  }

  delete computer;

  if(errorCount == 0)
    printf("LDA Unit Test: PERFECT SCORE\n");
  else
    printf("LDA Unit Test: Total number of errors found: %d\n", errorCount);

}
