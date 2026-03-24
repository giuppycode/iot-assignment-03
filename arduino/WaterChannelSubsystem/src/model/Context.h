#ifndef __CONTEXT__
#define __CONTEXT__

class Context
{

public:
  Context();

  void setAutomatic();
  bool isAutomatic();

  void setManual();
  bool isManual();

  void setUnconnected();
  bool isUnconnected();

  void setPotValue(int value);
  int getPotValue();

  void setCurrentDistance(float dist);
  float getCurrentDistance();

private:
  bool automatic;
  bool manual;
  bool unconnected;
  int potValue;
  float currentDistance;
};

#endif