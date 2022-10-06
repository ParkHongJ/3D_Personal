#include "stdafx.h"
#include "..\Default\Imgui\imgui.h"
#include "..\Default\Imgui\imgui_impl_dx11.h"
#include "..\Default\Imgui\imgui_impl_win32.h"

#include "..\Public\ImGui_Manager.h"
#include <corecrt_io.h>//file
#include <commdlg.h>//ofn
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

vector<pair<string, ID3D11ShaderResourceView*>> CImGui_Manager::resources = vector<pair<string, ID3D11ShaderResourceView*>>();

CImGui_Manager::CImGui_Manager()
{
}


HRESULT CImGui_Manager::Init(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsLight();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
																//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice,pContext);

	//test
	/*vMatrix = XMMatrixIdentity();
	XMVECTOR test = XMLoadFloat3(&vScale);
	XMVECTOR test1 = XMLoadFloat3(&vRotation);
	XMVECTOR test2 = XMLoadFloat3(&vPosition);
	XMMatrixDecompose(&test, &test1, &test2, vMatrix);
	XMStoreFloat3(&vScale, test);
	XMStoreFloat3(&vRotation, test1);
	XMStoreFloat3(&vPosition, test2);*/
	return S_OK;

}


void CImGui_Manager::RenderBegin()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	ImGui::ShowDemoWindow();
	
}

void CImGui_Manager::Render()
{
	ImGui::Begin("External Tool", 0, ImGuiWindowFlags_NoScrollbar);

	//Theme
	ImGuiStyle* style = &ImGui::GetStyle();

	style->WindowTitleAlign = ImVec2(0.5, 0.5);
	style->WindowMinSize = ImVec2(500, 430);

	style->Colors[ImGuiCol_TitleBg] = ImColor(255, 101, 53, 255);
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(255, 101, 53, 255);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImColor(0, 0, 0, 130);

	style->Colors[ImGuiCol_Button] = ImColor(31, 30, 31, 255);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(31, 30, 31, 255);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(41, 40, 41, 255);

	style->Colors[ImGuiCol_Separator] = ImColor(70, 70, 70, 255);
	style->Colors[ImGuiCol_SeparatorActive] = ImColor(76, 76, 76, 255);
	style->Colors[ImGuiCol_SeparatorHovered] = ImColor(76, 76, 76, 255);

	style->Colors[ImGuiCol_FrameBg] = ImColor(37, 36, 37, 255);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(37, 36, 37, 255);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(37, 36, 37, 255);

	style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_Header] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(46, 46, 46, 255);
	//------------

	ImGui::Columns(2);
	ImGui::SetColumnOffset(1, 230);

	static ImVec4 active = { (_uint)(41 / 255.0), (_uint)(40 / 255.0), (_uint)(41 / 255.0), (_uint)(255 / 255.0) };
	static ImVec4 inactive = { (_uint)(31 / 255.0), (_uint)(30 / 255.0), (_uint)(31 / 255.0), (_uint)(255 / 255.0) };

	{
		//LEFT SIDE
		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::MAP ? active : inactive);
		if (ImGui::Button("MapTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = MAP;

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::UNIT ? active : inactive);
		if (ImGui::Button("UnitTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = UNIT;

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::CAMERA ? active : inactive);
		if (ImGui::Button("CameraTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = CAMERA;

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::PARTICLE ? active : inactive);
		if (ImGui::Button("ParticleTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = PARTICLE;

		ImGui::PopStyleColor(4);
	}

	ImGui::NextColumn();
	{
		//RIGHT SIDE
		switch (m_eCurrentTool)
		{
		case CImGui_Manager::MAP:
		{
			if (!resources.empty())
			{
				ImGui::Image((void*)resources[item_current_idx].second, ImVec2(200, 200));
			}
			static ImGuiTextFilter filter;

			filter.Draw("Search", 240);

			if (ImGui::BeginListBox("ResourcesList", ImVec2(ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y - 35)))
			{
				for (_uint i = 0; i < resources.size(); i++)
				{
					if (filter.PassFilter(resources[i].first.c_str()))
					{
						const bool is_selected = (item_current_idx == i);
						if (ImGui::Selectable(resources[i].first.c_str(), is_selected))
						{
							item_current_idx = i;
						}

						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndListBox();
			}

			if (ImGui::Button("Save To Folder", ImVec2(205, 34)))
			{
				//Save Data

				OPENFILENAMEA ofn;
				CHAR szFile[260] = { 0 };
				ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
				ofn.lStructSize = sizeof(OPENFILENAMEA);
				ofn.hwndOwner = NULL;
				ofn.lpstrFile = szFile;
				ofn.nMaxFile = sizeof(szFile);

				//폴더 경로
				ofn.lpstrFilter = NULL;
				ofn.nFilterIndex = 1;
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
				if (GetSaveFileNameA(&ofn) == TRUE)
				{

				}
			}
			ImGui::SameLine();

			if (ImGui::Button("Load Image", ImVec2(205, 34)))
			{
				string temp = "..\\Bin\\Resources\\Textures\\Explosion\\*.*";
				struct _finddata_t c_file;

				intptr_t hFile = _findfirst(temp.c_str(), &c_file);
				if (hFile == -1) return;      // no files
				do
				{
					if (c_file.attrib & _A_SUBDIR)
					{
						// directory 처리
					}
					else if (c_file.attrib & _A_HIDDEN)
					{
						// hidden file 처리
					}
					else
					{
						// 일반 파일 처리

						_uint find = temp.rfind("\\") + 1;

						string filePath = temp.substr(0, find);
						//string fileName = sPath.substr(find, sPath.length() - find);
						filePath += c_file.name; // 불러와야할 파일이름

						//PDIRECT3DTEXTURE9 my_texture;
						//LoadTextureFileFromA(filePath.c_str(), &my_texture);

						//resources.push_back(make_pair(c_file.name, my_texture));

						//ImGui::Image((void*)texture, ImVec2(50, 50));
					}

				} while (_findnext(hFile, &c_file) == 0);

				_findclose(hFile);
			}

			//Get File
			//if (ImGui::Button("Load From Folder", ImVec2(205, 34)))
			//{
			//	//Load Data
			//	OPENFILENAMEA ofn;
			//	CHAR szFile[260] = { 0 };
			//	ZeroMemory(&ofn, sizeof(OPENFILENAMEA));
			//	ofn.lStructSize = sizeof(OPENFILENAMEA);
			//	ofn.hwndOwner = NULL;
			//	ofn.lpstrFile = szFile;
			//	ofn.nMaxFile = sizeof(szFile);

			//	//폴더 경로
			//	ofn.lpstrFilter = NULL;
			//	ofn.nFilterIndex = 1;
			//	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
			//	if (GetOpenFileNameA(&ofn) == TRUE)
			//	{
			//		int a = 10;
			//	}
			//}
		}
		break;
		case CImGui_Manager::UNIT:
		{
			if (m_pPrototypeGameObject == nullptr)
			{
				CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
				m_pPrototypeGameObject = pGameinstance->GetPrototypeGameObject();
				RELEASE_INSTANCE(CGameInstance);
			}

			if (m_pPrototypeComponent == nullptr)
			{
				CGameInstance* pGameinstance = GET_INSTANCE(CGameInstance);
				m_pPrototypeComponent = pGameinstance->GetPrototypeComponent(LEVEL_GAMEPLAY);
				RELEASE_INSTANCE(CGameInstance);
			}

			static char ObjName[MAX_PATH] = "";
			static char PrototypeName[MAX_PATH] = "";
			static char LayerName[MAX_PATH] = "";
			static char ModelName[MAX_PATH] = "";

			//szName
			ImGui::InputTextWithHint("OBJ Name", "szName", ObjName, IM_ARRAYSIZE(ObjName));

			//ModelComponent
			if (ImGui::BeginCombo("PrototypeGameObject", PrototypeName))
			{
				if (m_pPrototypeGameObject == nullptr)
				{
					ImGui::EndCombo();
					break;
				}

				for (auto& Pair : *m_pPrototypeGameObject)
				{
					char szPrototypeName[MAX_PATH] = "";
					WideCharToMultiByte(CP_ACP, 0, Pair.first, wcslen(Pair.first), szPrototypeName, MAX_PATH, 0, 0);

					if (nullptr != strstr(szPrototypeName, "_GameObject_"))
					{
						if (ImGui::Selectable(szPrototypeName, TRUE))
						{
							strcpy_s(PrototypeName, szPrototypeName);
						}
					}
				}
				ImGui::EndCombo();
			}
			//LayerName
			ImGui::InputTextWithHint("Layer Name", "Layer", LayerName, IM_ARRAYSIZE(LayerName));

			//ModelComponent
			if (ImGui::BeginCombo("ModelComponent", ModelName))
			{
				if (m_pPrototypeComponent == nullptr)
				{
					ImGui::EndCombo();
					break;
				}

				for (auto& Pair : *m_pPrototypeComponent)
				{
					char szPrototypeName[MAX_PATH] = "";
					WideCharToMultiByte(CP_ACP, 0, Pair.first, wcslen(Pair.first), szPrototypeName, MAX_PATH, 0, 0);

					if (nullptr != strstr(szPrototypeName, "Component_Model"))
					{
						if (ImGui::Selectable(szPrototypeName, TRUE))
						{
							strcpy_s(ModelName, szPrototypeName);
						}
					}
				}
				ImGui::EndCombo();
			}
			//게임오브젝트 추가하기
			if (ImGui::Button("AddGameObject"))
				ImGui::OpenPopup("AddObject");

			if (ImGui::BeginPopup("AddObject"))
			{
				if (ImGui::Button("StaticObject"))
				{
					//생성할때 필요한 자료.
					//프로토타입 태그
					//레이어 태그
					//레벨
					//모델명
					CREATE_INFO tObjInfo;
					ZeroMemory(&tObjInfo, sizeof(CREATE_INFO));
					
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, PrototypeName, strlen(PrototypeName), tObjInfo.pPrototypeTag, MAX_PATH);
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, LayerName, strlen(LayerName), tObjInfo.pLayerTag, MAX_PATH);
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ModelName, strlen(ModelName), tObjInfo.pModelTag, MAX_PATH);

					tObjInfo.iNumLevel = LEVEL_GAMEPLAY;
					sprintf_s(tObjInfo.szName, ObjName);

					AddGameObject(tObjInfo.pPrototypeTag, tObjInfo.pLayerTag, tObjInfo.iNumLevel, &tObjInfo);
				}

				ImGui::Separator();

				if (ImGui::Button("TestPopupButton1"))
				{

				}
				ImGui::EndPopup();
			}
		}
		break;
		case CImGui_Manager::CAMERA:
			break;
		case CImGui_Manager::PARTICLE:

			ImGui::DragFloat("Duration", &m_fDuration, 0.005f);

			ImGui::Spacing();
			ImGui::SameLine(420);
			ImGui::Checkbox("Looping", &m_bLooping);

			ImGui::DragFloat("Start Delay", &m_fStartDelay, 0.005f);

			ImGui::DragFloat("Start Lifetime", &m_fStartLifeTime, 0.005f);

			ImGui::DragFloat("Start Speed", &m_fStartSpeed, 0.005f);

			ImGui::Text("Start Size");


			break;
		default:
			break;
		}
	}

	ImGui::End();

	ImGui::Begin("Hierarchy");
	{
		//레이어 받아오기
		if (m_pHierarchyList == nullptr)
		{
			m_pHierarchyList = CGameInstance::Get_Instance()->GetLayers(LEVEL_GAMEPLAY);
		}
				

		//레이어 리스트 트리뷰 표시
		if (m_pHierarchyList != nullptr)
		{
			for (auto& Pair : *m_pHierarchyList)
			{
				list<class CGameObject*>* m_pLeafHierachyList = Pair.second->Get_Layer();

				char szWideFullPath[MAX_PATH] = "";
				WideCharToMultiByte(CP_ACP, 0, Pair.first, wcslen(Pair.first), szWideFullPath, MAX_PATH, 0, 0);
				
				if (ImGui::TreeNode(szWideFullPath))
				{
					if (m_pLeafHierachyList != nullptr)
					{
						for (auto& List : *m_pLeafHierachyList)
						{
							if (ImGui::Button(List->GetName()))
							{
								m_pSelectedObject = List;
								vMatrix = XMMatrixIdentity();
								CTransform* pTransform = (CTransform*)m_pSelectedObject->Get_ComponentPtr(L"Com_Transform");
								
								if (pTransform != nullptr)
								{
									vMatrix = pTransform->Get_WorldMatrix();

									XMVECTOR test;
									XMVECTOR test1;
									XMVECTOR test2;
									XMMatrixDecompose(&test, &test1, &test2, vMatrix);
									XMStoreFloat3(&vScale, test);
									XMStoreFloat3(&vRotation, test1);
									XMStoreFloat3(&vPosition, test2);
								}
							}
						}
					}
					ImGui::TreePop();
				}
			}
		}
		ImGui::End();
	}

	ImGui::Begin("Inspector");
	{
		if (m_pSelectedObject != nullptr)
		{
			/*=============
			===Transform===
			=============*/
			vPos[0] = vPosition.x; vPos[1] = vPosition.y; vPos[2] = vPosition.z;
			vRot[0] = vRotation.x; vRot[1] = vRotation.y; vRot[2] = vRotation.z;
			vScal[0] = vScale.x; vScal[1] = vScale.y; vScal[2] = vScale.z;
						
			ImGui::DragFloat3("Position", vPos, 0.01f, -FLT_MAX, FLT_MAX);
			ImGui::DragFloat3("Rotation", vRot, 0.01f, -FLT_MAX, FLT_MAX);
			ImGui::DragFloat3("Scale", vScal, 0.01f, -FLT_MAX, FLT_MAX);

			vPosition.x = vPos[0]; vPosition.y = vPos[1]; vPosition.z = vPos[2];
			vRotation.x = vRot[0]; vRotation.y = vRot[1]; vRotation.z = vRot[2];
			vScale.x = vScal[0]; vScale.y = vScal[1]; vScale.z = vScal[2];

			XMVECTOR XmPosition = XMLoadFloat3(&vPosition);
			XMVECTOR XmScale = XMLoadFloat3(&vScale);

			_matrix WorldMatrix = XMMatrixIdentity() * 
				XMMatrixScalingFromVector(XmScale) * 
				XMMatrixRotationX(XMConvertToRadians(vRotation.x)) *
				XMMatrixRotationY(XMConvertToRadians(vRotation.y)) *
				XMMatrixRotationZ(XMConvertToRadians(vRotation.z)) *
				XMMatrixTranslationFromVector(XmPosition);

			CTransform* pTransform = (CTransform*)m_pSelectedObject->Get_ComponentPtr(L"Com_Transform");
			if (pTransform != nullptr)
			{
				pTransform->Set_WorldMatrix(WorldMatrix);
			}
		}
		ImGui::End();
	}
}

void CImGui_Manager::Free()
{
	m_pHierarchyList = nullptr;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	/*	CleanupDeviceD3D();
	::DestroyWindow(hWnd);
	::UnregisterClass(wc.lpszClassName, wc.hInstance);*/

}

bool CImGui_Manager::LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv)
{
	// Load from disk into a raw RGBA buffer
	int image_width = 0;
	int image_height = 0;
	//unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	//if (image_data == NULL)
		//return false;

	//DirectX::CreateWICTextureFromFile(m_pDevice, filename, nullptr, &out_srv);

	return true;
}

HRESULT CImGui_Manager::AddGameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, _uint iNumLevel, void* pArg)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);
	
	if (FAILED(pGameInstance->Add_GameObjectToLayer(pPrototypeTag, iNumLevel, L"StaticObject", pArg)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

void CImGui_Manager::RenderEnd()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Update and Render additional Platform Windows
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}
