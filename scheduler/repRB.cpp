#define OUT_ROT    "data/rots.txt"
#define OUT_BLOCKS "data/blocks.txt"
#define OUT_RB     "data/rb.txt"
#define OUT_CONST  "data/const.txt"
#define OUT_RES    "data/res.txt"

#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[])
{
  streambuf *coutbuf = cout.rdbuf(); //save old buf
  int i, j, numRotations, numResidents, numBlocks;

  // -=-=-=-=-=-=-=-=-=-=-=-=- ROTS.txt -=-=-=-=-=-=-=-=-=-=-=-=- 
  ofstream out(OUT_ROT);
  cout.rdbuf(out.rdbuf()); 

  i = 1;
  cout << i++ << ",Neuro (9S)" << endl;
  cout << i++ << ",Pulmonary (8S)" << endl;
  cout << i++ << ",Heme (5E)" << endl; 
  cout << i++ << ",Adol-Renal (5W)" << endl; /**
  cout << i++ << ",ICS (4W CSH)" << endl;
  cout << i++ << ",PCR" << endl;
  cout << i++ << ",Dev-Advoc" << endl;
  cout << i++ << ",WBN" << endl;
  cout << i++ << ",HUP NICU" << endl;
  cout << i++ << ",ED" << endl;
  cout << i++ << ",Elective" << endl; **/
  cout << i++ << ",E Neuro" << endl;
  cout << i++ << ",E Pulm" << endl;
  cout << i++ << ",E Heme" << endl; 
  //cout << i++ << ",E Adol-Renal" << endl;
  //cout << i++ << ",E ICS" << endl;
  //cout << i++ << ",Flex" << endl; 

  numRotations = i-1;

  out.close();

  // -=-=-=-=-=-=-=-=-=-=-=-=- BLOCKS.txt -=-=-=-=-=-=-=-=-=-=-=-=- 
  out = ofstream(OUT_BLOCKS);
  cout.rdbuf(out.rdbuf()); 

  i = 0;
  cout << i++ << ",7/1,7/27" << endl;
  cout << i++ << ",7/28,8/24" << endl;
  cout << i++ << ",8/25,9/21" << endl;
  cout << i++ << ",9/22,10/19" << endl; 
  cout << i++ << ",10/20,11/16" << endl; /**
  cout << i++ << ",11/17,12/14" << endl;
  cout << i++ << ",12/15,1/11" << endl;
  cout << i++ << ",1/12,2/8" << endl;
  cout << i++ << ",2/9,3/8" << endl;
  cout << i++ << ",3/9,4/5" << endl;
  cout << i++ << ",4/6,5/3" << endl;
  cout << i++ << ",5/4,5/31" << endl;
  cout << i++ << ",6/1,6/30" << endl; **/

  numBlocks = i;

  out.close();

  // -=-=-=-=-=-=-=-=-=-=-=-=- RB.txt -=-=-=-=-=-=-=-=-=-=-=-=- 
  out = ofstream(OUT_RB);
  cout.rdbuf(out.rdbuf()); 

  for (i=0; i<numBlocks; i++) {

    j=1;
    cout << i << "," << j++ << ",3,3" << endl; // Neuro
    cout << i << "," << j++ << ",3,3" << endl; // Pulm
    cout << i << "," << j++ << ",3,3" << endl; // Heme 
    cout << i << "," << j++ << ",3,3" << endl; // Adol-Renal
    /** cout << i << "," << j++ << ",4,4" << endl; // ICS 
    cout << i << "," << j++ << ",3,4" << endl; // PCR
    cout << i << "," << j++ << ",3,4" << endl; // Dev-Adv
    cout << i << "," << j++ << ",3,4" << endl; // WBN
    cout << i << "," << j++ << ",4,4" << endl; // ICN
    cout << i << "," << j++ << ",3,4" << endl; // ED
    cout << i << "," << j++ << ",3,4" << endl; // Elective **/
    cout << i << "," << j++ << ",1,1" << endl; // E Neuro
    cout << i << "," << j++ << ",1,1" << endl; // E Pulm
    cout << i << "," << j++ << ",1,1" << endl; // E Heme
    //cout << i << "," << j++ << ",1,1" << endl; // E Adol-Renal
    //cout << i << "," << j++ << ",1,1" << endl; // E ICS
    //cout << i << "," << j++ << ",-1,-1" << endl; // Flex 

  }

  out.close();
  
  // -=-=-=-=-=-=-=-=-=-=-=-=- RES.txt -=-=-=-=-=-=-=-=-=-=-=-=- 
  out = ofstream(OUT_RES);
  cout.rdbuf(out.rdbuf()); 

  i = 0;
  cout << i++ << ",Danielle,Barber" << endl;
  cout << i++ << ",Meghan,Bullock" << endl;
  cout << i++ << ",Sarah,Catalano" << endl;
  cout << i++ << ",Ana,Cristancho" << endl;
  cout << i++ << ",George,Dalembert" << endl;
  cout << i++ << ",Thomas,Dietzman" << endl;
  cout << i++ << ",Briget,DiPrisco" << endl;
  cout << i++ << ",Emily,Echevarria" << endl;
  cout << i++ << ",Jonathan,Edelson" << endl;
  cout << i++ << ",Andrew,Edmondson" << endl;
  cout << i++ << ",Julie,Fierro" << endl;
  cout << i++ << ",Sara,Fridinger" << endl; 
  cout << i++ << ",Candace,Gildner" << endl;
  cout << i++ << ",Sarah,Ginsburg" << endl;
  cout << i++ << ",Scott,Gordon" << endl; /**
  cout << i++ << ",Rachel,Gottlieb-Smith" << endl; 
  cout << i++ << ",Katherine,Guttmann" << endl;
  cout << i++ << ",Tanya,Haj-Hassan" << endl;
  cout << i++ << ",Oze,Henig" << endl;
  cout << i++ << ",David,Hill" << endl;
  cout << i++ << ",Ann,Hsieh" << endl;
  cout << i++ << ",Stacey,Kallem" << endl;
  cout << i++ << ",Vinay,Kampalath" << endl;
  cout << i++ << ",Joelle,Karlik" << endl; 
  cout << i++ << ",Alexander,Lankowski" << endl;
  cout << i++ << ",Beatriz,Larru" << endl;
  cout << i++ << ",Naomi,Lewin" << endl;
  cout << i++ << ",Jamie,Lovell" << endl;
  cout << i++ << ",Melissa,Martin" << endl;
  cout << i++ << ",Dena,Matalon" << endl;
  cout << i++ << ",Steven,McKee" << endl;
  cout << i++ << ",Erika,Mejia" << endl;
  cout << i++ << ",Laura,Melnick" << endl;
  cout << i++ << ",Krysten,North" << endl;
  cout << i++ << ",Evan,Orenstein" << endl;
  cout << i++ << ",Vidya,Pai" << endl;
  cout << i++ << ",Hareem,Park" << endl;
  cout << i++ << ",Trusha,Patel" << endl;
  cout << i++ << ",Lisa,Ravener" << endl;
  cout << i++ << ",Laura,Robinson" << endl;
  cout << i++ << ",Julie,Rogers" << endl;
  cout << i++ << ",Margaret,Rush" << endl;
  cout << i++ << ",Jennifer,Salant" << endl;
  cout << i++ << ",Eloise,Salmon" << endl;
  cout << i++ << ",Lauren,Sanchez" << endl;
  cout << i++ << ",Margaret,Sarezky" << endl;
  cout << i++ << ",Christopher,Smith" << endl;
  cout << i++ << ",Stephanie,Smith" << endl;
  cout << i++ << ",Jessica,Tang" << endl;
  cout << i++ << ",Meredith,Winter" << endl; **/

  numResidents = i;

  out.close();

  // -=-=-=-=-=-=-=-=-=-=-=-=- CONST.txt -=-=-=-=-=-=-=-=-=-=-=-=- 
  out = ofstream(OUT_CONST);
  cout.rdbuf(out.rdbuf()); 

  for (i=0; i<numResidents; i++) {

    cout << i << ",1" << endl;

//    cout << i << ",0,1&2&3&4&5&6&7&8&9&10&11" << endl;
//    cout << i << ",2,12|13|14|15|16,1|2|3|4|5|9|12|13|14|15|16|17" << endl;

    cout << i << ",0,1&2&3&4" << endl;
    cout << i << ",2,5|6|7" << endl;
  }

  out.close();

  cout.rdbuf(coutbuf); //reset to standard output again

  return 0;
}