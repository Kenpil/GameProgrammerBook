#ifndef ___Array2D
#define ___Array2D

template< class T > class Array2D {
public:
	Array2D() : mArray(0) {}
	~Array2D() {
		delete[] mArray;
		mArray = 0;  //�|�C���^��0������̂̓N�Z�ɂ��悤�B
	}
	void setSize(int size0, int size1) {
		mSize0 = size0;
		mSize1 = size1;
		mArray = new T[size0 * size1];
	}
	T& operator()(int index0, int index1) {
		return mArray[index1 * mSize0 + index0];
	}
	const T& operator()(int index0, int index1) const {
		return mArray[index1 * mSize0 + index0];
	}
private:
	T* mArray;
	int mSize0;
	int mSize1;
};

#endif