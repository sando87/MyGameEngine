#pragma once
#include "junks.h"
#include "jRectangle2D.h"

template<typename Ty>
class jGridContainer
{
public:
	jGridContainer() {}
	~jGridContainer() {}

	u64 ToKey(Vector2 pos);
	u64 ToIndex(double pos);
	double ToPosition(u32 idx);
	Vector2 ToPosition(u64 key);
	bool IsContains(u64 key);
	bool IsContains(Vector2 pos);
	void GetItems(jRectangle2D rect, vector<Ty>& items);
	Ty & operator[](Vector2 pos);
	Ty & operator[](u64 key);
	void Clear() { Grids.clear(); }
	unordered_map<u64, Ty>& GetGrids() { return Grids; }

protected:
	unordered_map<u64, Ty> Grids;

	Property_GetSetter(double, Step, 1)
};

template<typename Ty>
inline u64 jGridContainer<Ty>::ToKey(Vector2 pos)
{
	return (u64)((ToIndex(pos.y) << 32) | ToIndex(pos.x));
}

template<typename Ty>
inline u64 jGridContainer<Ty>::ToIndex(double pos)
{
	return (u64)((pos + mStep / 2) / mStep);
}

template<typename Ty>
inline double jGridContainer<Ty>::ToPosition(u32 idx)
{
	return mStep * idx;
}

template<typename Ty>
inline Vector2 jGridContainer<Ty>::ToPosition(u64 key)
{
	return Vector2(ToPosition((u32)key), ToPosition((u32)(key >> 32)));
}

template<typename Ty>
inline bool jGridContainer<Ty>::IsContains(u64 key)
{
	return Grids.find(key) != Grids.end();
}

template<typename Ty>
inline bool jGridContainer<Ty>::IsContains(Vector2 pos)
{
	return Grids.find(ToKey(pos)) != Grids.end();
}

template<typename Ty>
inline void jGridContainer<Ty>::GetItems(jRectangle2D rect, vector<Ty>& items)
{
	vector<Vector2> points;
	rect.ClipGrid(mStep, points);
	for (Vector2 point : points)
	{
		u64 key = ToKey(point);
		if (IsContains(key))
			items.push_back(Grids[key]);
	}
}

template<typename Ty>
inline Ty & jGridContainer<Ty>::operator[](Vector2 pos)
{
	u64 key = ToKey(pos);
	if (!IsContains(key))
		Grids[key] = Ty();
	return Grids[key];
}

template<typename Ty>
inline Ty & jGridContainer<Ty>::operator[](u64 key)
{
	if (!IsContains(key))
		Grids[key] = Ty();
	return Grids[key];
}
