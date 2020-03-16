#pragma once

/** Calculates baseline temperature.

This is a protothread that averages 10 samples of data to determine the "initial" or "baseline" temperature of the tree. It should be used before the heater is turned on.

@param pt A pointer to the protothread control structure. The default parameter is correct. Don't forget to initialize the control structure in setup().
@returns the status of the protothread (Waiting, yeilded, exited, or ended)
*/
int baseline(struct pt *pt = &baseline_thd);
