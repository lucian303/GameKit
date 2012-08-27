#ifndef ANIMATE_H
#define ANIMATE_H

#include "..\h\gkit.h"
#include "..\h\fixed.h"

class AnimationNode {
	friend class AnimationRef;
	friend class Animation;
	protected:

	//public:
	Bitmap * Picture;
	AnimationNode * prev;
	AnimationNode * next;
	int Duration;
	//int Counter;

	public:
	AnimationNode();
	~AnimationNode();
	AnimationNode(char * file, AnimationNode * p, AnimationNode * n, int d);
	AnimationNode (Bitmap * AddBitmap, AnimationNode * p, AnimationNode * n, int d);

	void ErasePicture ();

	void ReAssign (Bitmap * AddBitmap, AnimationNode * p, AnimationNode * n, int d);
	void AnimationNode::ReAssign (char * file, AnimationNode * p, AnimationNode * n, int d);

	//void ResetCounter ();
	//void AssignCounter (int c );
	void ResetDuration ();
	void AssignDuration (int d);

};

class Animation;

class AnimationRef {
	protected:
	AnimationNode * thisone;
	int Duration;
	int Counter;

	public:

	AnimationRef();
	AnimationRef (Animation * Anim);
	void AssignAnimation(Animation * Anim);

	void Advance ();
	void Advance(int howmuch);

	void AdvanceFrame ();
	void AdvanceFrame(int howmuch);

	void PutRegular (UINT32 x, UINT32 y, char * offs);
	void PutTransparent (UINT32 x, UINT32 y, char * offs);
	void PutRegular18bit (UINT32 x, UINT32 y, char * offs);
	void PutTransparent18bit (UINT32 x, UINT32 y, char * offs);

	void PutTransparentFaded (UINT32 x, UINT32 y, char * offs,
       INT32 rfade, INT32 gfade, INT32 bfade);

	void PutRegularFaded (UINT32 x, UINT32 y, char * offs,
	   INT32 rfade, INT32 gfade, INT32 bfade);

	int GetWidth () { return  thisone->Picture->GetWidth();	};
	int GetHeight () { return  thisone->Picture->GetHeight();};


    void PutTransparentSemi (UINT32 x, UINT32 y, char * offs, float rtrans, float gtrans, float btrans);
    void PutRegularSemi (UINT32 x, UINT32 y, char * offs, float rtrans, float gtrans, float btrans);

	void PutTransparentOneCol (UINT32 x, UINT32 y, char * offs, char thecol);
    void PutTransparentOneCol (UINT32 x, UINT32 y, char * offs, int r, int g, int b);

};


class Animation : public AnimationRef {
	friend class AnimationRef;
	protected:

	//public:
	AnimationNode * first;
	AnimationNode * last;

	UINT32 NumFrames;

	public:
	Animation ();
	~Animation ();
	Animation (int nf, char * thefiles [], int Durs []);

	void Advance ();
	void Advance(int howmuch);
	void AssignAnimation(); //doesn't do anything!!!
	void Add (AnimationNode * AddMe, int d);
	void Add (char * file, int d);
	void AddAll (int nframes, char * files [], int Durs []);
	void RemoveLast ();

};



#endif

