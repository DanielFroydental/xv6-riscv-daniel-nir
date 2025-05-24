#define NPETERSON 16  // maximum number of Peterson locks

struct petersonlock {
  int valid;           // Is this lock slot valid/allocated?
  volatile int turn;   // Whose turn is it? (0 or 1)
  volatile int flag[2]; // Process intentions: flag[i] = 1 means process i wants to enter
};

extern struct petersonlock peterson_locks[NPETERSON];
