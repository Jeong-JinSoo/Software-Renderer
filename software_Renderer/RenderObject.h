#pragma once
#include <vector>			// 벌텍시스랑 인디시스관리

#include "RenderStruct.hpp" // transform, Vertex 상속.

class Camera;				// 카메라 전방선언

enum class RenderType
{
	NONE,
	Line,
	Face,
};

/// <summary>
/// 렌더링 오브젝트 관리를 위한 오브젝트
/// 기본적인 렌더링 오브젝트에 필요한 항목들을 미리 설정.
/// 큐브같은건 상속으로 만들지만, .obj파일같은걸 읽어와서 만들경우는 이걸 사용하기.
/// </summary>
class RenderObject
{
public:
	RenderObject();
	virtual	~RenderObject();

	void Initialize();
	void Finalize();

	// 오브젝트의 트랜스폼과 버텍스를 업데이트
	virtual void Update();
	
	// 오브젝트의 투영나누기 이전 W값을 가져옴(특정인덱스)
	virtual float GetPrevClipW(int index);

	// 오브젝트의 투영나누기 이전 W값을 가져옴(전체)
	std::vector<float>* GetPrevClipW();

	// 특정 인덱스의 버텍스 값을 반환함
	virtual int ReturnIndex(int i);

	virtual int GetTriangleCount();

private:
	virtual Vertex UpdateVertex(int index);
	
public:
	// 오브젝트의 트렌스폼
	// 오브젝트의 pos, rot, sca도 얘가 가지고 있음.
	Transform transform;

	// 오브젝트의 버텍스들
	std::vector<Vertex> vertecies;

	// 오브젝트의 인덱스들
	std::vector<int> indices;

	// 클립스페이스에서 투영나누기 전, 버텍스의 w 포지션 기록용
	std::vector<float> prevClipW;

	// 카메라의 포인터. 기존 view,proj,viewport를 매 오브젝트마다 가져왔지만
	// 생각해보니 어차피 카메라가 가지고 업데이트마다 갱신하지 않나 싶어서 카메라만.
	// view, proj, veiwport는 매번 카메라에서 가져오자.
	Camera* pcam;

	Matrix finalTransform; // 최종행렬
	std::vector<Vertex> finalVertex; // 최종행렬 먹인 버텍스
	Matrix normalMatrix;

	// 렌더링을 어떤 모양으로 할지 결정
	RenderType type;
};

