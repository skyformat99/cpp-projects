@echo off

del *.obj
del *.exe
del *.tlh
del *.tli

cl /nologo %1 %2 /EHsc repRB.cpp
del repRB.obj

cl /nologo %1 %2 /EHsc /c Utilities.cpp PropertyFile.cpp ResidentPool.cpp Resident.cpp BlockPool.cpp Block.cpp Rotation.cpp RotationPool.cpp RotationBlock.cpp RotationBlockPool.cpp ScheduleState.cpp Constraint.cpp ConstraintRequired.cpp ConstraintRepeat.cpp ConstraintRequiredSet.cpp

cl /nologo %1 %2 /EHsc test.cpp /link *.obj 