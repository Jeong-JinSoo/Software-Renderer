#pragma once
#include <vector>			// ���ؽý��� �ε�ý�����

#include "RenderStruct.hpp" // transform, Vertex ���.

class Camera;				// ī�޶� ���漱��

enum class RenderType
{
	NONE,
	Line,
	Face,
};

/// <summary>
/// ������ ������Ʈ ������ ���� ������Ʈ
/// �⺻���� ������ ������Ʈ�� �ʿ��� �׸���� �̸� ����.
/// ť�갰���� ������� ��������, .obj���ϰ����� �о�ͼ� ������� �̰� ����ϱ�.
/// </summary>
class RenderObject
{
public:
	RenderObject();
	virtual	~RenderObject();

	void Initialize();
	void Finalize();

	// ������Ʈ�� Ʈ�������� ���ؽ��� ������Ʈ
	virtual void Update();
	
	// ������Ʈ�� ���������� ���� W���� ������(Ư���ε���)
	virtual float GetPrevClipW(int index);

	// ������Ʈ�� ���������� ���� W���� ������(��ü)
	std::vector<float>* GetPrevClipW();

	// Ư�� �ε����� ���ؽ� ���� ��ȯ��
	virtual int ReturnIndex(int i);

	virtual int GetTriangleCount();

private:
	virtual Vertex UpdateVertex(int index);
	
public:
	// ������Ʈ�� Ʈ������
	// ������Ʈ�� pos, rot, sca�� �갡 ������ ����.
	Transform transform;

	// ������Ʈ�� ���ؽ���
	std::vector<Vertex> vertecies;

	// ������Ʈ�� �ε�����
	std::vector<int> indices;

	// Ŭ�������̽����� ���������� ��, ���ؽ��� w ������ ��Ͽ�
	std::vector<float> prevClipW;

	// ī�޶��� ������. ���� view,proj,viewport�� �� ������Ʈ���� ����������
	// �����غ��� ������ ī�޶� ������ ������Ʈ���� �������� �ʳ� �; ī�޶�.
	// view, proj, veiwport�� �Ź� ī�޶󿡼� ��������.
	Camera* pcam;

	Matrix finalTransform; // �������
	std::vector<Vertex> finalVertex; // ������� ���� ���ؽ�
	Matrix normalMatrix;

	// �������� � ������� ���� ����
	RenderType type;
};

