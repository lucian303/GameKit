//ANIMATE.CPP
//Animation functions
#include <string.h>
#include "..\h\gkit.h"
#include "..\h\fixed.h"
#include "..\h\bitmap.h"
#include "..\h\animate.h"


AnimationNode::~AnimationNode() {
 	ErasePicture ();
}


AnimationNode::AnimationNode () {
	Picture = NULL;
	prev = next = NULL;
	Picture = new Bitmap;
	AssignDuration(0);
	//AssignCounter(0);
}

AnimationNode::AnimationNode(char * file, AnimationNode * p, AnimationNode * n, int d) {
	Picture = NULL;
	ReAssign (file, p, n, d);
}

AnimationNode::AnimationNode (Bitmap * AddBitmap, AnimationNode * p, AnimationNode * n, int d) {
	Picture = NULL;
	ReAssign (AddBitmap, p, n, d);
}

//void AnimationNode::ResetCounter () {Counter = 0;}
//void AnimationNode::AssignCounter (int c ) {Counter = c;}
void AnimationNode::ResetDuration () {Duration = 0;}
void AnimationNode::AssignDuration (int d) {Duration = d;}


void AnimationNode::ErasePicture () {
	if ( Picture) {
		delete Picture;
	}

}

void AnimationNode::ReAssign (Bitmap * AddBitmap, AnimationNode * p, AnimationNode * n, int d) {
	AssignDuration(d);
	//AssignCounter(0);
	Picture = NULL;
	Picture = AddBitmap;
	prev = p;
	next = n;
}

void AnimationNode::ReAssign (char * file, AnimationNode * p, AnimationNode * n, int d) {
	AssignDuration(d);
	//AssignCounter(0);
	Picture = NULL;
	Picture = new Bitmap;
	Picture->Load(file);
	prev = p;
	next = n;
}



//-----------------------------------------------------------------------------



Animation::Animation () {
	first = last = thisone = NULL;
	NumFrames = 0;
	Duration = 0;
	Counter = 0;
}

Animation::Animation (int nf, char * thefiles [], int Durs []) {
	Counter = 0;
	AddAll (nf, thefiles, Durs);
}

Animation::~Animation () {

  AnimationNode * nodeCounter, * nextNodeCounter;
  if (first != last)
  {
    for (nodeCounter = first; nodeCounter != last; nodeCounter = nextNodeCounter)
    {
      nextNodeCounter = nodeCounter->next;
      delete nodeCounter;
    }
    delete last;
  }
  if ((first == last) && (first != NULL))
    RemoveLast();

  first = last = thisone = NULL;
  NumFrames = 0;
}

void Animation::Add (AnimationNode * AddMe, int d) {
	NumFrames++;

	if (first != NULL)
  	{
    	last->next = AddMe; // make the new node
    	last->next->prev = last; //make new node point back
    	first->prev = last;
    	last = last->next;
  	}
  	else
  	{
    	first = AddMe; // make the new node
    	last = first;
    	last->prev = first; //make new node point back
    	first->prev = last; //so it wraps backwards too.
    	thisone = first; // look at the first frame for anim purposes.
  	}
//-------------------------------------------------------------------
   last->AssignDuration(d);
	//last->prev->AssignCounter(0);


	if (last->next)
    	Add(last->next, d);
  	else
    	last->next = first;



}

void Animation::Add (char * file, int d) {

  NumFrames++;

  if (first != NULL)
  {
    last->next = new AnimationNode; // make the new node
    last->next->prev = last; //make new node point back
    last = last->next; //make tail point to new node
    last->next = first; // make last  point to head
    last->Picture->Load(file);
    first->prev = last;
  }
  else
  {
    first = new AnimationNode; // make the new node
    last = first;
    last->prev = first; //make new node point back
    last->next = first; // make last  point to head
	 last->Picture->Load(file);
    first->prev = last; //so it wraps backwards too.
    thisone = first; // look at the first  for anim purposes.
  }

   last->AssignDuration(d);
	//last->prev->AssignCounter(0);

}

void Animation::RemoveLast () {

  if (first != last)
  {
    last = last->prev;
    delete (last->next);
    last->next = first;
    first->prev = last;
  }
  else if (first == last)
  {
    delete first;
    first = last = thisone = NULL;
  }
  NumFrames--;


}


void Animation::AddAll (int nframes, char * files [], int Durs []) {

	for ( int tru = 0; tru < nframes; tru ++) {
		Add (files[tru], Durs[tru]);
	}
}

void Animation::AssignAnimation() {

}

/*void Animation::Advance () {

  //static int counter;

  Counter++;
  if ( thisone->Counter >= thisone->Duration) {
	Counter = 0;

	if (thisone != NULL) {
		thisone = thisone->next;
		Counter = 0;
		Duration = thisone->Duration;
	}
  }

} */



void AnimationRef::Advance () {

  //static int counter;

  Counter++;
  if ( Counter >= Duration) {
	Counter = 0;

	if (thisone != NULL) {
		thisone = thisone->next;
		//Counter = 0;
		Duration = thisone->Duration;
	}
  }

}

void AnimationRef::Advance(int howmuch) {
  for (int counter = 0; counter < howmuch; ++counter)
    Advance();
}

void AnimationRef::AdvanceFrame () {

	if (thisone != NULL) {
		thisone = thisone->next;
		Duration = thisone->Duration;
		Counter = 0;
	}

}


void AnimationRef::AdvanceFrame(int howmuch) {
  for (int counter = 0; counter < howmuch; ++counter)
    AdvanceFrame();
}

void AnimationRef::PutRegular (UINT32 x, UINT32 y, char * offs) {
	thisone->Picture->PutRegular(x,y,offs);
}

void AnimationRef::PutTransparent (UINT32 x, UINT32 y, char * offs) {
		thisone->Picture->PutTransparent(x,y,offs);
}

void AnimationRef::PutRegular18bit (UINT32 x, UINT32 y, char * offs) {
	thisone->Picture->PutRegular18bit(x,y,offs);
}
void AnimationRef::PutTransparent18bit (UINT32 x, UINT32 y, char * offs) {
		thisone->Picture->PutTransparent18bit(x,y,offs);
}

void AnimationRef::PutTransparentFaded (UINT32 x, UINT32 y, char * offs, INT32 rfade, INT32 gfade, INT32 bfade) {
	thisone->Picture->PutTransparentFaded(x,y,offs, rfade, bfade, gfade);
}

void AnimationRef::PutRegularFaded (UINT32 x, UINT32 y, char * offs, INT32 rfade, INT32 gfade, INT32 bfade) {
	thisone->Picture->PutRegularFaded(x,y,offs, rfade, bfade, gfade);
}

void AnimationRef::PutRegularSemi (UINT32 x, UINT32 y, char * offs, float rtrans, float gtrans, float btrans) {
	thisone->Picture->PutRegularSemi(x,y,offs, rtrans, btrans, gtrans);
}

void AnimationRef::PutTransparentSemi (UINT32 x, UINT32 y, char * offs, float rtrans, float gtrans, float btrans) {

	thisone->Picture->PutTransparentSemi(x,y,offs, rtrans, btrans, gtrans);
}

void AnimationRef::PutTransparentOneCol (UINT32 x, UINT32 y, char * offs, char thecol) {
	thisone->Picture->PutTransparentOneCol (x, y, offs, thecol);
}

void AnimationRef::PutTransparentOneCol (UINT32 x, UINT32 y, char * offs, int r, int g, int b) {
   thisone->Picture->PutTransparentOneCol (x, y, offs, r, g, b);
}

AnimationRef::AnimationRef() {
	thisone = NULL;
	Counter = 0;
	Duration = 0;
}

AnimationRef::AnimationRef(Animation * Anim) {
	AssignAnimation (Anim);
}

void AnimationRef::AssignAnimation(Animation * Anim) {
	Counter = 0;
	thisone = Anim->first;
	Duration = thisone->Duration;
}



