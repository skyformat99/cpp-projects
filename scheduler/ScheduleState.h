#ifndef __ScheduleState_h__
#define __ScheduleState_h__

// Forward declarations
class ResidentPool;
class BlockPool;
class RotationPool;
class RotationBlockPool;
class Resident;
class Block;
class Rotation;

class ScheduleState
{

public:
  ScheduleState (ResidentPool*, BlockPool*, RotationPool*, RotationBlockPool*);
  ~ScheduleState ();

  bool areAllFilled ();
  bool areRotationBlocksWithinLimits ();
  bool areRotationBlocksWithinLimits (Resident*, Block*, Rotation*, 
           int, bool careAboutMin = false);

  Rotation* getFilledRotation (int, int);

  bool isResidentAssigned (int, int);
  bool isRotationSurroundingBlock (int, int, int);

  void randomFill ();
  void print (); 
  void printByBlockRotation ();
  void reset ();

  Resident* getNextRequiredResident ();
  Resident* getNextResident ();
  Block* getResidentEmptyBlock (Resident*);
  Block* getResidentEmptyBlockSpecial (Resident*);

  void assign (Resident*, Block*, Rotation*);
  void unassign (Resident*, Block*, Rotation*);

  bool isResidentRequiredSetDone (Resident*, int);
  void setResidentRequiredSetDone (Resident*, int, bool);

private:

  int** state;

  ResidentPool* rp;
  BlockPool* bp;
  RotationPool* op;

  RotationBlockPool* rbp;
  int** rotationBlockCount;

  int* residentFilled;
  int* residentRequiredFilled;

  bool noMoreRequired;

  bool** residentRequiredSetDone;

};

#endif