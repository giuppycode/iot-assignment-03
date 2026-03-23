#ifndef __CONTEXT__
#define __CONTEXT__

class Context
{

public:
  Context();

  void setCurrentDistance(float dist);
  float getCurrentDistance();

private:
  float currentDistance;
};

#endif