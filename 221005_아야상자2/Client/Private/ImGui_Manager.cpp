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
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_MAP ? active : inactive);
		if (ImGui::Button("MapTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_MAP;

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_UNIT ? active : inactive);
		if (ImGui::Button("UnitTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_UNIT;

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_CAMERA ? active : inactive);
		if (ImGui::Button("CameraTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_CAMERA;

		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_PARTICLE ? active : inactive);
		if (ImGui::Button("ParticleTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_PARTICLE;

		ImGui::PushStyleColor(ImGuiCol_Button, m_eCurrentTool == CImGui_Manager::TOOL_ANIMATION ? active : inactive);
		if (ImGui::Button("AnimationTool", ImVec2(230 - 15, 41)))
			m_eCurrentTool = TOOL_ANIMATION;
		ImGui::PopStyleColor(5);
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
					CREATE_INFO tObjInfo;
					ZeroMemory(&tObjInfo, sizeof(CREATE_INFO));

					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, PrototypeName, (_int)strlen(PrototypeName), tObjInfo.pPrototypeTag, MAX_PATH);
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, LayerName, (_int)strlen(LayerName), tObjInfo.pLayerTag, MAX_PATH);
					MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, ModelName, (_int)strlen(ModelName), tObjInfo.pModelTag, MAX_PATH);

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
//		{
//			if (nullptr == m_pSelectedObject)
//				break;
//			m_pModel = (CModel*)m_pSelectedObject->Get_ComponentPtr(L"Com_Model");
//			if (nullptr == m_pModel)
//				break;
//			m_pAnimations = m_pModel->GetAnimations();
//			if (nullptr == m_pAnimations)
//				break;
//
//			//선택한 오브젝트의 모든 애니메이션들
//			ImGui::BeginListBox("Animations", ImVec2(400, 150));
//			for (_uint i = 0; i < m_pAnimations->size(); ++i)
//			{
//				const bool is_selected = (Animation_current_idx == i);
//				//선택했다면
//				if (ImGui::Selectable((*m_pAnimations)[i]->GetName(), true))
//				{
//					//현재 인덱스를 바꿔주고 애니메이션을 플레이함
//					Animation_current_idx = i;
//					m_pModel->Change_Animation(Animation_current_idx);
//
//
//					//선택한 애니메이션의 정보 갱신
//					//SelectedCurrentAnim.name = (*m_pAnimations)[i]->GetName();
//					//SelectedCurrentAnim.iNextIndex = Animation_current_idx;
//
//					SelectedNextAnim.name = (*m_pAnimations)[i]->GetName();
//					SelectedNextAnim.iNextIndex = Animation_current_idx;
//				}
//			}
//			ImGui::EndListBox();
//			ImGui::SameLine();
//			char temp[MAX_PATH] = "";
//			strcpy_s(temp, (*m_pAnimations)[Animation_current_idx]->GetName());
//			ImGui::InputText("testText : ", temp, MAX_PATH);
//
//			map<string, _uint>::iterator iter;
//			 
//			
//
//			ImGui::BeginListBox("CurrentAnim", ImVec2(400, 150));
//
//			_uint iSelectedIndex = 0;
//			static string SelectedName;
//			
//			for (iter = m_CurrentAnim.begin(); iter != m_CurrentAnim.end(); iter++)
//			{
//				const bool is_selected = (Animation_Edit_Idx == iSelectedIndex);
//				if (ImGui::Selectable(iter->first.c_str(), true))
//				{
//					//Animation_Edit_Idx = iSelectedIndex;
//					(*m_pAnimations)[iSelectedIndex]->GetAnimationInfo(m_fBlendTime, m_bLoop, m_bHasExitTime);
//
//					//현재 편집할 애니메이션의 정보를 갱신
//					//SelectedCurrentAnim.name = iter->first;
//					//SelectedCurrentAnim.iNextIndex = Animation_current_idx;
//					SelectedCurrentAnim.name = iter->first;
//					SelectedCurrentAnim.iNextIndex = Animation_Edit_Idx = iter->second;
//				}
//				iSelectedIndex++;
//			}
//
//
//			ImGui::EndListBox();
//			ImGui::SameLine();
//
//			ImGui::BeginListBox("NextAnim", ImVec2(400, 150));
//
//			static _uint iSelectedNextIndex = 0;
//			for (_uint i = 0; i < m_ResultPair[SelectedCurrentAnim.iNextIndex].second.size(); i++)
//			{
//				const bool is_selected = (iSelectedNextIndex == i);
//				if (ImGui::Selectable(m_ResultPair[SelectedCurrentAnim.iNextIndex].second[i].name.c_str(), true))
//				{
//					iSelectedNextIndex = i;
//				}
//			}
//			ImGui::EndListBox();
//
//			ImGui::Text("CurrentAnim : ");
//			ImGui::SameLine();
//			ImGui::Text(SelectedCurrentAnim.name.c_str());
//			ImGui::SameLine();
//			ImGui::Text("NextAnim : ");
//			ImGui::SameLine();
//			ImGui::Text(SelectedNextAnim.name.c_str());
//
//			/* 현재 애니메이션을 편집할 리스트박스에 넣음. */
//			if (ImGui::Button("AddCurrentAnim"))
//			{
//				iter = m_CurrentAnim.find((*m_pAnimations)[Animation_current_idx]->GetName());
//				//겹치는게 없다면 넣음
//				if (iter == m_CurrentAnim.end())
//				{
//					//선택한 애니메이션의 이름과, 그 애니메이션의 인덱스번호를 넘겨줌
//					m_CurrentAnim.insert({SelectedNextAnim.name, SelectedNextAnim.iNextIndex}/*{ (*m_pAnimations)[Animation_current_idx]->GetName(), Animation_current_idx }*/);
//				}
//			}
//
//			ImGui::SameLine();
//
//			if (ImGui::Button("AddNextAnim"))
//			{
//				iter = m_CurrentAnim.find((*m_pAnimations)[Animation_current_idx]->GetName());
//				//겹치는게 없다면 넣음
//				if (iter == m_CurrentAnim.end() || SelectedCurrentAnim.name != "")
//				{
//					//선택한 애니메이션의 이름과, 그 애니메이션의 인덱스번호를 넘겨줌
//					//m_NextAnim.insert(make_pair(SelectedNextAnim.name, SelectedNextAnim.iNextIndex));
//
//					m_ResultPair[SelectedCurrentAnim.iNextIndex].first = SelectedCurrentAnim;
//					ANIM_INFO animInfo;
//					animInfo.name = SelectedNextAnim.name;
//					animInfo.iNextAnimIndex = SelectedNextAnim.iNextIndex;
//
//					m_ResultPair[SelectedCurrentAnim.iNextIndex].second.push_back(animInfo);
//					//m_ResultPair[SelectedCurrentAnim.iNextIndex].second.push_back(SelectedNextAnim);
//				}
//			}
//
//			ImGui::Checkbox("Loop", &m_bLoop);
//			ImGui::SameLine();
//			ImGui::Checkbox("HasExitTime", &m_bHasExitTime);
//
//			ImGui::DragFloat("BlendTime", &m_fBlendTime, 0.01f, 0.0f, 5.0f);
//
//			ImGui::InputText("Message", buf, MAX_PATH);
//			
//			if (m_ResultPair[Animation_Edit_Idx].first.name != "")
//			{
//				ImGui::Text(m_ResultPair[Animation_Edit_Idx].first.name.c_str());
//			}
//			ImGui::SameLine();
//			if (!m_ResultPair[Animation_Edit_Idx].second.empty())
//			{
//				ImGui::Text(m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].name.c_str());
//			}
//			if (ImGui::Button("Apply"))
//			{
//				//strcpy_s(m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].message, buf);
//				m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].message = buf;
//				m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].fBlendTime = m_fBlendTime;
//				m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].bLoop = m_bLoop;
//				m_ResultPair[Animation_Edit_Idx].second[iSelectedNextIndex].bHasExitTime = m_bHasExitTime;
//
//				ZeroMemory(&buf, MAX_PATH);
//			}
//
//			ImGui::BeginListBox("CompletedAnim", ImVec2(400, 150));
//			ImGui::EndListBox();
//
//			if (ImGui::Button("TestXMLBUTTON"))
//			{
//				//출처 : https://blog.naver.com/PostView.naver?isHttpsRedirect=true&blogId=chansung0602&logNo=221014997196
//				
//				//xml 선언
//				TiXmlDocument doc;
//				TiXmlDeclaration* pDec1 = new TiXmlDeclaration("1.0", "", "");
//				doc.LinkEndChild(pDec1);
//
//				//루트 노드 추가
//				//하나의 노드를 다루기 위한 변수, 이를 xml 파일로 종속시킨다.
//				TiXmlElement* pRoot = new TiXmlElement("AnimationDB");
//				doc.LinkEndChild(pRoot);
//
//				//주석 문장 추가
//				TiXmlComment* pComment = new TiXmlComment();
//				pComment->SetValue("This is Animation DB");
//				pRoot->LinkEndChild(pComment);
//
//				_uint i = 0;
//				while (true)
//				{
//					if (m_ResultPair[i].first.name == "")
//					{
//						break;
//					}
//					// 하위노드 및 데이터 추가
//					TiXmlElement* pElem = new TiXmlElement("Animations");
//					pRoot->LinkEndChild(pElem);
//					TiXmlElement* pElem0 = new TiXmlElement("CurrentAnimName");
//					pElem0->LinkEndChild(new TiXmlElement(m_ResultPair[i].first.name.c_str()));
//					pElem->LinkEndChild(pElem0);
//
//					//하위노드 및 속성 추가
//					TiXmlElement* pSubElem = new TiXmlElement("NextAnimInfo");
//					pElem->LinkEndChild(pSubElem);
//					for (_uint j = 0; j < m_ResultPair[i].second.size(); ++j)
//					{
//						pSubElem->SetAttribute("name", m_ResultPair[i].second[j].name.c_str());
//						pSubElem->SetAttribute("Message", m_ResultPair[i].second[j].message.c_str());
//						pSubElem->SetAttribute("bHasExitTime", m_ResultPair[i].second[j].bHasExitTime);
////						pSubElem->SetAttribute("fBlendTime", m_ResultPair[i].second[j].fBlendTime);
//					}
//
//					
//					i++;
//				}
//				doc.SaveFile("../Bin/Data/Test.xml");
//			}
//		}
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

HRESULT CImGui_Manager::AddGameObject(const _tchar * pPrototypeTag, const _tchar * pLayerTag, _uint iNumLevel, void* pArg)
{
	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(pGameInstance->Add_GameObjectToLayer(pPrototypeTag, iNumLevel, L"StaticObject", pArg)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;

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
