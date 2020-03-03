#ifndef INCLUDED_STATE_H
#define INCLUDED_STATE_H

#include "Array2D.h"

class Image; //�錾�������Ă����΂����B�C���N���[�h���Ȃ��Ă����B

//��ԃN���X
class State{
private:
	class Object;
public:
	State( const char* stageData, int size );
	~State();
	void update( int moveX, int moveY );
	void draw() const;
	//void drawCell(int x, int y, Object::ImageID id) const;
	bool hasCleared() const;
private:
	void setSize( const char* stageData, int size );

	int mWidth;
	int mHeight;
	Array2D<Object> mObjects;
	Image* mImage; //�摜�f�[�^�̎󂯓n���B�|�C���^������Object������A�N�Z�X�ł���
	int mMoveCount;
};

#endif
