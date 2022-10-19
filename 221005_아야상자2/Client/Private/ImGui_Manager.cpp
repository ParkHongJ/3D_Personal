#include "stdafx.h"
#include "..\Default\Imgui\imgui.h"
#include "..\Default\Imgui\imgui_impl_dx11.h"
#include "..\Default\Imgui\imgui_impl_win32.h"

#include "..\Public\ImGui_Manager.h"
#include "tinyxml.h"
#include <corecrt_io.h>//file
#include <commdlg.h>//ofn
#include "GameInstance.h"
#include "GameObject.h"
#include "Layer.h"
#include "Model.h"
#include "Animation.h"
#include "Cell.h"
#include <string.h>
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

	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\malgun.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesKorean());
	
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(pDevice, pContext);
	
	SelectedCurrentAnim.iNextIndex = 0;
	SelectedCurrentAnim.name = "";
	SelectedNextAnim.iNextIndex = 0;
	SelectedNextAnim.name = "";
	
	m_ResultPair.resize(150);

	LoadObject();
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
		/*ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_MAP ? active : inactive);
		if (ImGui::Button("MapTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_MAP;*/

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_UNIT ? active : inactive);
		if (ImGui::Button("UnitTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_UNIT;

		/*ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_CAMERA ? active : inactive);
		if (ImGui::Button("CameraTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_CAMERA;

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_PARTICLE ? active : inactive);
		if (ImGui::Button("ParticleTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_PARTICLE;*/

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_ANIMATION ? active : inactive);
		if (ImGui::Button("AnimationTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_ANIMATION;

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_ANIMATION ? active : inactive);
		if (ImGui::Button("NavMeshTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_NAVIGATION;

		ImGui::PopStyleColor(3);
	}

	ImGui::NextColumn();
	{
		//RIGHT SIDE
		switch (m_eCurrentTool)
		{
		case CImGui_Manager::TOOL_MAP:
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

						_uint find = (_uint)temp.rfind("\\") + 1;

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
		case CImGui_Manager::TOOL_UNIT:
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
					WideCharToMultiByte(CP_ACP, 0, Pair.first, (_int)wcslen(Pair.first), szPrototypeName, MAX_PATH, 0, 0);

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
					WideCharToMultiByte(CP_ACP, 0, Pair.first, (_int)wcslen(Pair.first), szPrototypeName, MAX_PATH, 0, 0);

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
					//CREATE_INFO tObjInfo;
					//tObjInfo.pPrototypeTag = CharToWstring(PrototypeName);
					//tObjInfo.pLayerTag = CharToWstring(LayerName);
					//tObjInfo.pModelTag = CharToWstring(ModelName);
					///*MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, PrototypeName, (_int)strlen(PrototypeName), tObjInfo.pPrototypeTag, MAX_PATH);

					//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, LayerName, (_int)strlen(LayerName), tObjInfo.pLayerTag, MAX_PATH);
					//MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ModelName, (_int)strlen(ModelName), tObjInfo.pModelTag, MAX_PATH);*/
					//tObjInfo.iNumLevel = LEVEL_GAMEPLAY;
					////sprintf_s(tObjInfo.szName, ObjName);

					//m_CreateObj.push_back(tObjInfo);
					//AddGameObject(tObjInfo.pPrototypeTag.c_str(), tObjInfo.pLayerTag.c_str(), tObjInfo.iNumLevel, &tObjInfo);
					CREATE_INFO tObjInfo;
					ZeroMemory(&tObjInfo, sizeof(CREATE_INFO));

					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, PrototypeName, strlen(PrototypeName), tObjInfo.pPrototypeTag, MAX_PATH);
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, LayerName, strlen(LayerName), tObjInfo.pLayerTag, MAX_PATH);
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ModelName, strlen(ModelName), tObjInfo.pModelTag, MAX_PATH);

					tObjInfo.iNumLevel = LEVEL_GAMEPLAY;
					sprintf_s(tObjInfo.szName, ObjName);

					wsprintf(tObjInfo.pLayerTag, L"StaticObject");
					
					XMStoreFloat4x4(&tObjInfo.WorldMatrix, XMMatrixIdentity());

					AddGameObject(tObjInfo.pPrototypeTag, tObjInfo.pLayerTag, tObjInfo.iNumLevel, &tObjInfo);
					m_CreateObj.push_back(tObjInfo);
				}
				
				ImGui::Separator();

				if (ImGui::Button("TestPopupButton1"))
				{

				}
				ImGui::EndPopup();
			}
			if (ImGui::Button("Save"))
			{
				for (auto& Pair : *m_pHierarchyList)
				{
					if (wcscmp(Pair.first, L"StaticObject"))
					{
						continue;
					}
					list<class CGameObject*>* pLeafHierachyList = Pair.second->Get_Layer();
					for (auto& iter : *pLeafHierachyList)
					{
						for (auto& iter2 : m_CreateObj)
						{
							if (!strcmp(iter->GetName(), iter2.szName))
							{
								CTransform* pTransform = (CTransform*)iter->Get_ComponentPtr(L"Com_Transform");
								_float4x4 WorldMatrix;
								XMStoreFloat4x4(&WorldMatrix, pTransform->Get_WorldMatrix());
								iter2.WorldMatrix = WorldMatrix;
							}
						}
					}

				}

				_ulong		dwByte = 0;
				DWORD		dwStrByte = 0;

				HANDLE		hFile = CreateFile(TEXT("../Bin/Data/MapStaticInfo.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
				if (0 == hFile)
					return;

				/*
				char szName[260] = "";
				wchar_t pPrototypeTag[260] = L"";
				wchar_t pLayerTag[260] = L"";
				unsigned int iNumLevel = LEVEL_END;
				wchar_t pModelTag[260] = L"";

				XMFLOAT4X4 WorldMatrix;
				*/
				//이름저장
				for (auto& iter : m_CreateObj)
				{
					//Prototype
					dwStrByte = DWORD(sizeof(_tchar) * wcslen(iter.pPrototypeTag));
					WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
					WriteFile(hFile, &iter.pPrototypeTag, dwStrByte, &dwByte, nullptr);

					//Layer
					dwStrByte = DWORD(sizeof(_tchar) * wcslen(iter.pLayerTag));
					WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
					WriteFile(hFile, &iter.pLayerTag, dwStrByte, &dwByte, nullptr);

					//Model
					dwStrByte = DWORD(sizeof(_tchar) * wcslen(iter.pModelTag));
					WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
					WriteFile(hFile, &iter.pModelTag, dwStrByte, &dwByte, nullptr);

					//name
					dwStrByte = DWORD(sizeof(_char) * strlen(iter.szName));
					WriteFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
					WriteFile(hFile, &iter.szName, dwStrByte, &dwByte, nullptr);

					//Level
					WriteFile(hFile, &iter.iNumLevel, sizeof(_uint), &dwByte, nullptr);

					//Matrix
					WriteFile(hFile, &iter.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);
				}

				CloseHandle(hFile);
			}
		}
		break;
		case CImGui_Manager::TOOL_CAMERA:
			break;
		case CImGui_Manager::TOOL_PARTICLE:
			ImGui::DragFloat("Duration", &m_fDuration, 0.005f);

			ImGui::Spacing();
			ImGui::SameLine(420);
			ImGui::Checkbox("Looping", &m_bLooping);

			ImGui::DragFloat("Start Delay", &m_fStartDelay, 0.005f);

			ImGui::DragFloat("Start Lifetime", &m_fStartLifeTime, 0.005f);

			ImGui::DragFloat("Start Speed", &m_fStartSpeed, 0.005f);

			ImGui::Text("Start Size");
			break;
		case CImGui_Manager::TOOL_ANIMATION:
		{
			if (nullptr == m_pSelectedObject)
				break;
			m_pModel = (CModel*)m_pSelectedObject->Get_ComponentPtr(L"Com_Model");
			if (nullptr == m_pModel)
				break;
			m_pAnimations = m_pModel->GetAnimations();
			if (nullptr == m_pAnimations)
				break;

			//선택한 오브젝트의 모든 애니메이션들
			ImGui::BeginListBox("Animations", ImVec2(400, 150));
			for (_uint i = 0; i < m_pAnimations->size(); ++i)
			{
				const bool is_selected = (Animation_current_idx == i);
				//선택했다면
				if (ImGui::Selectable((*m_pAnimations)[i]->GetName(), true))
				{
					//현재 인덱스를 바꿔주고 애니메이션을 플레이함
					Animation_current_idx = i;
					m_pModel->Change_Animation(Animation_current_idx);


					//선택한 애니메이션의 정보 갱신
					//SelectedCurrentAnim.name = (*m_pAnimations)[i]->GetName();
					//SelectedCurrentAnim.iNextIndex = Animation_current_idx;

					SelectedNextAnim.name = (*m_pAnimations)[i]->GetName();
					SelectedNextAnim.iNextIndex = Animation_current_idx;
				}
			}
			ImGui::EndListBox();

			ImGui::SameLine();
			char temp[MAX_PATH] = "";
			strcpy_s(temp, (*m_pAnimations)[Animation_current_idx]->GetName());
			ImGui::InputText("testText : ", temp, MAX_PATH);

			map<string, _uint>::iterator iter;
			 
			

			ImGui::BeginListBox("CurrentAnim", ImVec2(400, 150));

			_uint iSelectedIndex = 0;
			static string SelectedName;
			
			for (iter = m_CurrentAnim.begin(); iter != m_CurrentAnim.end(); iter++)
			{
				const bool is_selected = (Animation_Edit_Idx == iSelectedIndex);
				if (ImGui::Selectable(iter->first.c_str(), true))
				{
					//Animation_Edit_Idx = iSelectedIndex;
					(*m_pAnimations)[iSelectedIndex]->GetAnimationInfo(m_fBlendTime, m_bLoop, m_bHasExitTime);

					//현재 편집할 애니메이션의 정보를 갱신
					//SelectedCurrentAnim.name = iter->first;
					//SelectedCurrentAnim.iNextIndex = Animation_current_idx;
					SelectedCurrentAnim.name = iter->first;
					SelectedCurrentAnim.iNextIndex = Animation_Edit_Idx = iter->second;
				}
				iSelectedIndex++;
			}


			ImGui::EndListBox();
			ImGui::SameLine();

			/*ImGui::BeginListBox("NextAnim", ImVec2(400, 150));

			static _uint iSelectedNextIndex = 0;
			for (_uint i = 0; i < m_ResultPair[SelectedCurrentAnim.iNextIndex].second.size(); i++)
			{
				const bool is_selected = (iSelectedNextIndex == i);
				if (ImGui::Selectable(m_ResultPair[SelectedCurrentAnim.iNextIndex].second[i].name.c_str(), true))
				{
					iSelectedNextIndex = i;
				}
			}
			ImGui::EndListBox();
*/
			//ImGui::Text("CurrentAnim : ");
			//ImGui::SameLine();
			//ImGui::Text(SelectedCurrentAnim.name.c_str());
			//ImGui::SameLine();
			//ImGui::Text("NextAnim : ");
			//ImGui::SameLine();
			//ImGui::Text(SelectedNextAnim.name.c_str());

			///* 현재 애니메이션을 편집할 리스트박스에 넣음. */
			//if (ImGui::Button("AddCurrentAnim"))
			//{
			//	iter = m_CurrentAnim.find((*m_pAnimations)[Animation_current_idx]->GetName());
			//	//겹치는게 없다면 넣음
			//	if (iter == m_CurrentAnim.end())
			//	{
			//		//선택한 애니메이션의 이름과, 그 애니메이션의 인덱스번호를 넘겨줌
			//		m_CurrentAnim.insert({SelectedNextAnim.name, SelectedNextAnim.iNextIndex}/*{ (*m_pAnimations)[Animation_current_idx]->GetName(), Animation_current_idx }*/);
			//	}
			//}

			//ImGui::SameLine();

			//if (ImGui::Button("AddNextAnim"))
			//{
			//	iter = m_CurrentAnim.find((*m_pAnimations)[Animation_current_idx]->GetName());
			//	//겹치는게 없다면 넣음
			//	if (iter == m_CurrentAnim.end() || SelectedCurrentAnim.name != "")
			//	{
			//		//선택한 애니메이션의 이름과, 그 애니메이션의 인덱스번호를 넘겨줌
			//		//m_NextAnim.insert(make_pair(SelectedNextAnim.name, SelectedNextAnim.iNextIndex));

			//		m_ResultPair[SelectedCurrentAnim.iNextIndex].first = SelectedCurrentAnim;
			//		ANIM_INFO animInfo;
			//		animInfo.name = SelectedNextAnim.name;
			//		animInfo.iNextAnimIndex = SelectedNextAnim.iNextIndex;

			//		m_ResultPair[SelectedCurrentAnim.iNextIndex].second.push_back(animInfo);
			//		//m_ResultPair[SelectedCurrentAnim.iNextIndex].second.push_back(SelectedNextAnim);
			//	}
			//}

			//ImGui::Checkbox("Loop", &m_bLoop);
			//ImGui::SameLine();
			//ImGui::Checkbox("HasExitTime", &m_bHasExitTime);

			//ImGui::DragFloat("BlendTime", &m_fBlendTime, 0.01f, 0.0f, 5.0f);

			//ImGui::InputText("Message", buf, MAX_PATH);
			//
			//if (m_ResultPair[Animation_Edit_Idx].first.name != "")
			//{
			//	ImGui::Text(m_ResultPair[Animation_Edit_Idx].first.name.c_str());
			//}
			//ImGui::SameLine();
			//if (!m_ResultPair[Animation_Edit_Idx].second.empty())
			//{
			//	ImGui::Text(m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].name.c_str());
			//}
			//if (ImGui::Button("Apply"))
			//{
			//	//strcpy_s(m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].message, buf);
			//	m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].message = buf;
			//	m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].fBlendTime = m_fBlendTime;
			//	m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].bLoop = m_bLoop;
			//	m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].bHasExitTime = m_bHasExitTime;

			//	ZeroMemory(&buf, MAX_PATH);
			//}

			//ImGui::BeginListBox("CompletedAnim", ImVec2(400, 150));
			//ImGui::EndListBox();

			if (ImGui::Button("TestXMLBUTTON"))
			{
				//출처 : https://blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=chansung0602&logNo=221014997196
				
				//xml 선언
				TiXmlDocument doc;
				TiXmlDeclaration* pDec1 = new TiXmlDeclaration("1.0", "", "");
				doc.LinkEndChild(pDec1);

				//루트 노드 추가
				//하나의 노드를 다루기 위한 변수, 이를 xml 파일로 종속시킨다.
				TiXmlElement* pRoot = new TiXmlElement("AnimationDB");
				doc.LinkEndChild(pRoot);

				//주석 문장 추가
				TiXmlComment* pComment = new TiXmlComment();
				pComment->SetValue("This is Animation DB");
				pRoot->LinkEndChild(pComment);

				_uint i = 0;
				while (true)
				{
					if (m_ResultPair[i].first.name == "")
					{
						break;
					}
					// 하위노드 및 데이터 추가
					TiXmlElement* pElem = new TiXmlElement("Animations");
					pRoot->LinkEndChild(pElem);
					TiXmlElement* pElem0 = new TiXmlElement("CurrentAnimName");
					pElem0->LinkEndChild(new TiXmlElement(m_ResultPair[i].first.name.c_str()));
					pElem->LinkEndChild(pElem0);

					//하위노드 및 속성 추가
					TiXmlElement* pSubElem = new TiXmlElement("NextAnimInfo");
					pElem->LinkEndChild(pSubElem);
					for (_uint j = 0; j < m_ResultPair[i].second.size(); ++j)
					{
						pSubElem->SetAttribute("name", m_ResultPair[i].second[j].name.c_str());
						pSubElem->SetAttribute("Message", m_ResultPair[i].second[j].message.c_str());
						pSubElem->SetAttribute("bHasExitTime", m_ResultPair[i].second[j].bHasExitTime);
//						pSubElem->SetAttribute("fBlendTime", m_ResultPair[i].second[j].fBlendTime);
					}

					
					i++;
				}
				doc.SaveFile("../Bin/Data/Test.xml");
			}
		}
		break;
		case CImGui_Manager::TOOL_NAVIGATION:
			ShowNavMesh();
		break;
		default:
			break;
		}
	}

	ImGui::End();

	ImGui::Begin("Hierarchy");
	ShowHierarchy();
	ImGui::End();

	ImGui::Begin("Inspector");
	Inspector();
	ImGui::End();

	
}

void CImGui_Manager::Free()
{
	m_pHierarchyList = nullptr;

	for (auto& pCell : m_Cells)
		Safe_Release(pCell);

	m_Cells.clear();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
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

void CImGui_Manager::PushPickPos(_fvector vPickPos)
{
	if (m_eNav != NAV_ADD)
		return;

	if (m_iCurrentNaviIndex <= 2)
	{
		XMStoreFloat3(&m_vPickPos[m_iCurrentNaviIndex++], vPickPos);
		if (m_iCurrentNaviIndex > 2)
		{
			CCell* pCell = CCell::Create(m_pDevice, m_pContext, m_vPickPos, (_int)m_Cells.size());
			m_Cells.push_back(pCell);
			m_iCurrentNaviIndex = 0;

			//삼각형을 만들었다면 기존의 선택한 셀들은 없앰
			/*for (auto& pCells : m_SelectCells)
			{
				Safe_Release(pCells);
			}
			m_SelectCells.clear();*/
		}
	}
}

void CImGui_Manager::DeletePickPos()
{
	XMStoreFloat3(&m_vPickPos[m_iCurrentNaviIndex--], XMVectorSet(0.f, 0.f, 0.f, 1.f));
	if (0 > m_iCurrentNaviIndex )
	{
		m_iCurrentNaviIndex = 0;
	}
}

void CImGui_Manager::ClearPickPos()
{
	for (_uint i = 0; i < 3; ++i)
	{
		XMStoreFloat3(&m_vPickPos[i], XMVectorSet(0.f, 0.f, 0.f, 1.f));
	}
	m_iCurrentNaviIndex = 0;
}

HRESULT CImGui_Manager::LoadObject()
{
	HANDLE		hFile = CreateFile(TEXT("../Bin/Data/MapStaticInfo.dat"), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	//읽은 바이트 
	DWORD	dwByte = 0;
	DWORD	dwStrByte = 0;

	//프로토타입
	//레이어
	//모델
	//이름(char)
	//레벨
	//매트릭스
	while (true)
	{
		//프로토타입 로드
		CREATE_INFO tObjInfo;
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pPrototypeTag, dwStrByte, &dwByte, nullptr);

		//레이어 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pLayerTag, dwStrByte, &dwByte, nullptr);

		//모델 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.pModelTag, dwStrByte, &dwByte, nullptr);

		//이름 로드
		ReadFile(hFile, &dwStrByte, sizeof(DWORD), &dwByte, nullptr);
		ReadFile(hFile, tObjInfo.szName, dwStrByte, &dwByte, nullptr);

		//레벨 로드
		ReadFile(hFile, &tObjInfo.iNumLevel, sizeof(_uint), &dwByte, nullptr);

		//매트릭스 로드
		ReadFile(hFile, &tObjInfo.WorldMatrix, sizeof(_float4x4), &dwByte, nullptr);

		if (0 == dwByte)
		{
			break;
		}
		m_CreateObj.push_back(tObjInfo);
	}

	CloseHandle(hFile);
	return S_OK;
}

void CImGui_Manager::RenderGizmo()
{
	for (auto& pCell : m_Cells)
	{
		if (nullptr != pCell)
			pCell->Render_Cell();
	}
	for (auto& pCell : m_SelectCellwithPoints)
	{
		if (nullptr != pCell.first)
		{
			pCell.first->Render_Cell(0.1f, _float4(1.f, 0.f, 1.f, 1.f));
		}
	}
	for (auto& pCell : m_SelectCell)
	{
		if (nullptr != pCell.first)
		{
			pCell.first->Render_Cell(0.1f, _float4(1.f, 0.f, 1.f, 1.f));
		}
	}
}

HRESULT CImGui_Manager::AddGameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, _uint iNumLevel, void* pArg)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pPrototypeTag, iNumLevel, L"StaticObject", pArg)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

}

void CImGui_Manager::ShowNavMesh() {
	static _int SelectMode = 0;
	ImGui::RadioButton("NAV_ADD", &SelectMode, 0); ImGui::SameLine();
	ImGui::RadioButton("NAV_EDIT_Point", &SelectMode, 1); ImGui::SameLine();
	ImGui::RadioButton("NAV_EDIT_Cell", &SelectMode, 2); ImGui::SameLine();
	m_eNav = (NavMesh)SelectMode;

	if (m_eNav == NAV_ADD)
	{
		for (auto& pCell : m_SelectCellwithPoints)
		{
			Safe_Release(pCell.first);
		}
		m_SelectCellwithPoints.clear();

		for (auto& pCell : m_SelectCell)
		{
			Safe_Release(pCell.first);
		}
		m_SelectCell.clear();

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		/*if (pGameInstance->Mouse_Down(DIMK_LBUTTON))
		{
			_uint iNumCell = 0;
			for (auto& pCells : m_Cells)
			{
				if (nullptr == pCells)
					continue;

				_bool is_Coll = pCells->Picking(&m_iNumPointIndex);
				if (is_Coll)
				{
					m_iNumCell = iNumCell;
				}
				iNumCell++;
			}
		}*/
		if (pGameInstance->Mouse_Down(DIMK_RBUTTON))
		{
			_uint iNumCell = 0;
			for (auto& pCells : m_Cells)
			{
				if (nullptr == pCells)
					continue;
				_bool is_Coll = pCells->Picking(&m_iNumPointIndex);

				//이미 배치되어있는 셀이 피킹이 되었을 때
				if (is_Coll)
				{
					m_iNumCell = iNumCell;
					PushPickPos(XMLoadFloat3(&pCells->Get_Point((CCell::POINT)m_iNumPointIndex)));
					break;
				}
				iNumCell++;
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	if (m_eNav == NAV_EDIT_POINT)
	{
		for (auto& pCell : m_SelectCell)
		{
			Safe_Release(pCell.first);
		}
		m_SelectCell.clear();

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (pGameInstance->Mouse_Down(DIMK_LBUTTON))
		{
			_uint iNumCell = 0;
			_bool bSelectCellsDelete = false;
			
			//한번만 실행하기 위한 변수
			_bool bCheck = false;
			for (auto& pCells : m_Cells)
			{
				if (nullptr == pCells)
					continue;

				//현재 셀의 포인트가 피킹 되었다면
				_bool is_Coll = pCells->Picking(&m_iNumPointIndex);
				if (is_Coll)
				{
					if (!bSelectCellsDelete)
					{
						for (auto& pCell : m_SelectCellwithPoints)
						{
							Safe_Release(pCell.first);
						}
						m_SelectCellwithPoints.clear();
					}
					bSelectCellsDelete = true;

					//일단 선택한 점을 갖고있는 1개의 셀을 등록한후
					m_iNumCell = iNumCell;

					if (!bCheck)
					{
						_uint iNumCurrentCell = 0;
						//모든 셀을 돌며 조사함
						for (auto& ppCells : m_Cells)
						{
							if (nullptr == ppCells)
							{
								iNumCurrentCell++;
								continue;
							}
							_uint iPointIndex = 0;
							if (true == ppCells->isInVertex(XMLoadFloat3(&pCells->Get_Point((CCell::POINT)m_iNumPointIndex)), &iPointIndex))
							{
								SELECT_CELL_POINT cell;
								//어떤 포인트를 피킹했는지
								cell.iPointIndex = iPointIndex;
								//진짜 Cell의 본체의 인덱스.
								cell.iOriginCellIndex = iNumCurrentCell;
								m_SelectCellwithPoints.push_back(make_pair(ppCells, cell));
								Safe_AddRef(ppCells);
							}
							iNumCurrentCell++;
						}
						bCheck = true;
					}
				}
				iNumCell++;
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	if (m_eNav == NAV_EDIT_CELL)
	{
		for (auto& pCell : m_SelectCellwithPoints)
		{
			Safe_Release(pCell.first);
		}
		m_SelectCellwithPoints.clear();

		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		if (pGameInstance->Mouse_Down(DIMK_LBUTTON) && pGameInstance->Key_Pressing(DIK_LSHIFT))
		{
			_uint iNumCell = 0;
			for (auto& pCells : m_Cells)
			{
				if (nullptr == pCells)
				{
					iNumCell++;
					continue;
				}
				//셀을 선택했다면
				_bool is_Coll = pCells->Picking();
				if (is_Coll)
				{
					m_iNumCell = iNumCell;
					//원본 셀의 인덱스를 저장해주고
					SELECT_CELL_POINT Cell;
					Cell.iOriginCellIndex = m_iNumCell;

					m_SelectCell.push_back(make_pair(pCells, Cell));
					Safe_AddRef(pCells);
				}
				iNumCell++;
			}
		}
		else if (pGameInstance->Mouse_Down(DIMK_LBUTTON))
		{
			_uint iNumCell = 0;
			for (auto& pCells : m_Cells)
			{
				if (nullptr == pCells)
				{
					iNumCell++;
					continue;
				}
				//셀을 선택했다면
				_bool is_Coll = pCells->Picking();
				if (is_Coll)
				{
					for (auto& pCell : m_SelectCell)
					{
						Safe_Release(pCell.first);
					}
					m_SelectCell.clear();

					m_iNumCell = iNumCell;

					//원본 셀의 인덱스를 저장해주고
					SELECT_CELL_POINT Cell;
					Cell.iOriginCellIndex = m_iNumCell;

					m_SelectCell.push_back(make_pair(pCells, Cell));
					Safe_AddRef(pCells);
				}
				iNumCell++;
			}
		}
		RELEASE_INSTANCE(CGameInstance);
	}

	if (ImGui::Button("TEST"))
	{
		for (auto& pCell : m_SelectCellwithPoints)
		{
			//first : Cell
			//second : +될 값
			pCell.first->EditCell(pCell.second.iPointIndex, _float3(0.f, 0.5f, 0.f));
		}
	}
	if (ImGui::Button("TESTType"))
	{
		for (auto& pCell : m_SelectCell)
		{
			//first : Cell
			//second : +될 값
			pCell.first->SetType(CCell::CANTMOVE);
		}
		for (auto& pCell : m_SelectCell)
		{
			Safe_Release(pCell.first);
		}
		m_SelectCell.clear();
	}

	if (ImGui::Button("Delete"))
	{
		for (auto& pCell : m_SelectCellwithPoints)
		{
			Safe_Release(m_Cells[pCell.second.iOriginCellIndex]);
			Safe_Release(pCell.first);
		}

		m_SelectCellwithPoints.clear();

		for (auto& pCell : m_SelectCell)
		{
			Safe_Release(m_Cells[pCell.second.iOriginCellIndex]);
			Safe_Release(pCell.first);
		}
		m_SelectCell.clear();

		m_Cells.erase(remove(m_Cells.begin(), m_Cells.end(), nullptr), m_Cells.end());
	}

	if (ImGui::Button("Clear"))
	{
		ZeroMemory(&m_vPickPos, sizeof(m_vPickPos));
	}

	if (ImGui::Button("SaveNav"))
	{
		_ulong		dwByte = 0;
		HANDLE		hFile = CreateFile(TEXT("../Bin/Data/NavigationDataTest.dat"), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
		if (0 == hFile)
			return;

		for (auto& pCell : m_Cells)
		{
			if (nullptr == pCell)
				continue;

			_float3* Points = pCell->GetPointArray();
			WriteFile(hFile, &Points[0], sizeof(_float3) * 3, &dwByte, nullptr);
		}

		CloseHandle(hFile);
	}
}

void CImGui_Manager::ShowHierarchy()
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
			WideCharToMultiByte(CP_ACP, 0, Pair.first, (_int)wcslen(Pair.first), szWideFullPath, MAX_PATH, 0, 0);

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
}

void CImGui_Manager::Inspector()
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
