#ifndef GUARD_CPU_H
#define GUARD_CPU_H

class CPU {
public:
    int read_id();
    CPU();
private:
    int id = 0;
};

#endif
