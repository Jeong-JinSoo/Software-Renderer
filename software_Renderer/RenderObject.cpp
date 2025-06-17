#include "RenderObject.h"
#include "Camera.h"

RenderObject::RenderObject()
	:pcam(nullptr)
{

}

RenderObject::~RenderObject()
{

}

void RenderObject::Initialize()
{
	int vert_num = vertecies.size();
	prevClipW.resize(vert_num);
	finalVertex.resize(vert_num);

	// ���� �������� ��쿣 �� ������ �н��Ѵ�.
	if (type == RenderType::Face)
	{
		// �븻 ���
		// �� �嵐���� ���� �� �븻�� ����ؼ� 3���� ������ �����ֱ�
		for (int i = 0; i < indices.size(); i += 3)
		{
			// �ε����� ���� �ﰢ���� ����(�� �ε���) ����
			int i0 = indices[i + 0];
			int i1 = indices[i + 1];
			int i2 = indices[i + 2];

			// ������ �ε����κ��� ���� ����
			Point A = vertecies[i0].vPoint;
			Point B = vertecies[i1].vPoint;
			Point C = vertecies[i2].vPoint;

			//�� ���� ���͸� ���Ѵ�.
			Point e1 = B - A;
			Point e2 = C - A;

			// �������� �븻 ���͸� ���Ѵ� -> ���� ���ͷ� ����ȭ
			// �÷� ���̵��� �븻�� ���� �� �ִ�.
			Point faceN = Point::Normalize(Point::Cross(e1, e2));

			vertecies[i0].vNormal += faceN;
			vertecies[i1].vNormal += faceN;
			vertecies[i2].vNormal += faceN;
		}

		// ��� ������ �븻 ���� ���ͷ� ����ȭ
		for (int i = 0; i < vert_num; i++)
		{
			vertecies[i].vNormal = Point::Normalize(vertecies[i].vNormal);
		}
	}

	// Ʈ������ �ʱ�ȭ
	transform.SetPosition(0, 0, 0);
	transform.SetRotation(0, 0, 0);
	transform.SetScale(1, 1, 1);
}

void RenderObject::Finalize()
{
}

void RenderObject::Update()
{
	Matrix camView = pcam->GetView();
	Matrix camProj = pcam->GetProj();
	Matrix viewport = pcam->GetViewPort();

	Matrix result;

	transform.Update();
	finalTransform = transform.GetTransformMatrix() * camView * camProj;

	// �븻 ����
	if (type == RenderType::Face)
	{

		// �𵨰� �� �ռ�
		Matrix modelView = transform.GetTransformMatrix() * camView;
		// �� �� ���� ���ϱ�
		modelView.Inverse();

		normalMatrix = modelView.Transposed();
	}

	int vertindex = vertecies.size();
	for (int i = 0 ; i < vertindex ; i++ )
	{
		finalVertex[i] = UpdateVertex(i);
	}
}

float RenderObject::GetPrevClipW(int index) const
{
	return prevClipW[index];
}

std::vector<float> RenderObject::GetPrevClipW() const
{
	return prevClipW;
}

Vertex RenderObject::UpdateVertex(int index)
{
	Vertex result;
	Point ver_result;
	Point nor_result = vertecies[index].vNormal;

	result.color = vertecies[index].color;

	/// ������ȯ
	if (index == -1)
	{
		// Wt * Vt * Pt ���� ���
		ver_result = ver_result * finalTransform;
	}

	else
	{
		ver_result = vertecies[index].vPoint * finalTransform;
		prevClipW[index] = ver_result.w;
	}

	// ���������� (Perspective Divide)
	ver_result.x /= ver_result.w;
	ver_result.y /= ver_result.w;
	ver_result.z /= ver_result.w;
	ver_result.w /= ver_result.w;

	// ����Ʈ ���ϱ�
	ver_result = ver_result * pcam->GetViewPort();

	/// �븻 ��ȯ
	// (inverse - tranpose -> ����ȭ)
	//normalMatrix = transpose(inverse(worldviewMatrix))
	nor_result = nor_result * normalMatrix;
	nor_result.Normalize(nor_result);

	result.vPoint = ver_result;
	result.vNormal = nor_result;

	return result;
}

int RenderObject::ReturnIndex(int i) const
{
	return indices[i];
}

int RenderObject::GetTriangleCount() const
{
	int result = indices.size() / 3;
	return result;
}
