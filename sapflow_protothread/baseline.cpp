// Calculates baseline temperature
int baseline(struct pt *pt)
{
  PT_BEGIN(pt);MARK;
  Serial.print("Initializing baseline thread... ");
  // Declare persistant variable for this thread
  static int i;
  // Initialize the baseline (reference) temperature
  reference.upper = 0;
  reference.lower = 0;
  maxtemp = -300; //< Any temperature should be greater than this.
  Serial.println("Done");
  // Take an average over the first 10 seconds
  for(i = 0; i < 10; ++i){ MARK;
    PT_WAIT_UNTIL(pt, sample_trigger); MARK;
    PT_WAIT_WHILE(pt, sample_trigger); MARK;
    reference.upper += latest.upper;
    reference.lower += latest.lower;
  }; MARK;
  reference.upper /= i;
  reference.lower /= i; MARK;
  cout<<"Baseline: "<<reference.upper<<", "<<reference.lower<<endl; MARK;
  PT_END(pt);
}
