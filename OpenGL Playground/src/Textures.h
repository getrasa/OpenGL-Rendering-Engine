#pragma once
#include <string>
#include <vector>

struct TextureInfo
{
	std::string meshName;
	std::string path;
	std::string textureType;
};

std::vector<TextureInfo> heroineTextures;


void assignTextures()
{
	TextureInfo textureInfo;

	//// Jacket
	// albedo
	textureInfo.meshName = "Mesh.017";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Jacket_BaseColor.tga";
	textureInfo.textureType = "albedoMap";
	heroineTextures.push_back(textureInfo);
	// normal
	textureInfo.meshName = "Mesh.017";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Jacket_Normal.tga";
	textureInfo.textureType = "normalMap";
	heroineTextures.push_back(textureInfo);
	// roughness
	textureInfo.meshName = "Mesh.017";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Jacket_Roughness.tga";
	textureInfo.textureType = "roughnessMap";
	heroineTextures.push_back(textureInfo);
	// metallic
	textureInfo.meshName = "Mesh.017";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Jacket_Metallic.tga";
	textureInfo.textureType = "metallicMap";
	heroineTextures.push_back(textureInfo);
	// opacity
	/*textureInfo.meshName = "Mesh.021";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/white_map.png";
	textureInfo.textureType = "opacityMap";
	heroineTextures.push_back(textureInfo);*/


	//// Hat
	// albedo
	textureInfo.meshName = "Mesh.018";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hat_BaseColor.tga";
	textureInfo.textureType = "albedoMap";
	heroineTextures.push_back(textureInfo);
	// normal
	textureInfo.meshName = "Mesh.018";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hat_Normal.tga";
	textureInfo.textureType = "normalMap";
	heroineTextures.push_back(textureInfo);
	// roughness
	textureInfo.meshName = "Mesh.018";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hat_Roughness.tga";
	textureInfo.textureType = "roughnessMap";
	heroineTextures.push_back(textureInfo);
	// metallic
	textureInfo.meshName = "Mesh.018";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hat_Metallic.tga";
	textureInfo.textureType = "metallicMap";
	heroineTextures.push_back(textureInfo);
	// opacity
	/*textureInfo.meshName = "Mesh.021";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/white_map.png";
	textureInfo.textureType = "opacityMap";
	heroineTextures.push_back(textureInfo);*/


	//// Hair 1
	// albedo
	textureInfo.meshName = "Mesh.020";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_BaseColor.tga";
	textureInfo.textureType = "albedoMap";
	heroineTextures.push_back(textureInfo);
	// normal
	textureInfo.meshName = "Mesh.020";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_Normal.tga";
	textureInfo.textureType = "normalMap";
	heroineTextures.push_back(textureInfo);
	// roughness
	/*textureInfo.meshName = "Mesh.018";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hat_Roughness.tga";
	textureInfo.textureType = "roughnessMap";
	heroineTextures.push_back(textureInfo);*/
	// metallic
	textureInfo.meshName = "Mesh.020";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	textureInfo.textureType = "metallicMap";
	heroineTextures.push_back(textureInfo);
	// opacity
	textureInfo.meshName = "Mesh.020";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_Opacity.tga";
	textureInfo.textureType = "opacityMap";
	heroineTextures.push_back(textureInfo);


	//// Hair 2
	// albedo
	textureInfo.meshName = "Mesh.020.1";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_BaseColor.tga";
	textureInfo.textureType = "albedoMap";
	heroineTextures.push_back(textureInfo);
	// normal
	textureInfo.meshName = "Mesh.020.1";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_Normal.tga";
	textureInfo.textureType = "normalMap";
	heroineTextures.push_back(textureInfo);
	// roughness
	/*textureInfo.meshName = "Mesh.018";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hat_Roughness.tga";
	textureInfo.textureType = "roughnessMap";
	heroineTextures.push_back(textureInfo);*/
	// metallic
	textureInfo.meshName = "Mesh.020.1";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	textureInfo.textureType = "metallicMap";
	heroineTextures.push_back(textureInfo);
	// opacity
	textureInfo.meshName = "Mesh.020.1";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_Opacity.tga";
	textureInfo.textureType = "opacityMap";
	heroineTextures.push_back(textureInfo);


	////// Eyes (was, now unknown)
	//// albedo
	//textureInfo.meshName = "Mesh.022";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Eyes_BaseColor.tga";
	//textureInfo.textureType = "albedoMap";
	//heroineTextures.push_back(textureInfo);
	//// normal
	//textureInfo.meshName = "Mesh.022";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Eyes_Normal.tga";
	//textureInfo.textureType = "normalMap";
	//heroineTextures.push_back(textureInfo);
	//// roughness
	//textureInfo.meshName = "Mesh.022";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Eyes_Roughness.tga";
	//textureInfo.textureType = "roughnessMap";
	//heroineTextures.push_back(textureInfo);
	//// metallic
	//textureInfo.meshName = "Mesh.022";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	//textureInfo.textureType = "metallicMap";
	//heroineTextures.push_back(textureInfo);
	//// opacity
	///*textureInfo.meshName = "Mesh.022";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	//textureInfo.textureType = "opacityMap";
	//heroineTextures.push_back(textureInfo);*/


	////// Eyes Shadow
	//// albedo
	//textureInfo.meshName = "Mesh.022.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Eyes_BaseColor.tga";
	//textureInfo.textureType = "albedoMap";
	//heroineTextures.push_back(textureInfo);
	//// normal
	//textureInfo.meshName = "Mesh.022.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Eyes_Normal.tga";
	//textureInfo.textureType = "normalMap";
	//heroineTextures.push_back(textureInfo);
	//// roughness
	//textureInfo.meshName = "Mesh.022.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Eyes_Roughness.tga";
	//textureInfo.textureType = "roughnessMap";
	//heroineTextures.push_back(textureInfo);
	//// metallic
	//textureInfo.meshName = "Mesh.022.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	//textureInfo.textureType = "metallicMap";
	//heroineTextures.push_back(textureInfo);
	//// opacity
	//textureInfo.meshName = "Mesh.022.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	//textureInfo.textureType = "opacityMap";
	//heroineTextures.push_back(textureInfo);


	////// Bikini
	//// albedo
	//textureInfo.meshName = "Mesh.016";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Bikini_BaseColor.tga";
	//textureInfo.textureType = "albedoMap";
	//heroineTextures.push_back(textureInfo);
	//// normal
	//textureInfo.meshName = "Mesh.016";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Bikini_Normal.tga";
	//textureInfo.textureType = "normalMap";
	//heroineTextures.push_back(textureInfo);
	//// roughness
	//textureInfo.meshName = "Mesh.016";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Bikini_Roughness.tga";
	//textureInfo.textureType = "roughnessMap";
	//heroineTextures.push_back(textureInfo);
	//// metallic
	//textureInfo.meshName = "Mesh.016";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Bikini_Metallic.tga";
	//textureInfo.textureType = "metallicMap";
	//heroineTextures.push_back(textureInfo);
	//// opacity
	///*textureInfo.meshName = "Mesh.016";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/white_map.png";
	//textureInfo.textureType = "opacityMap";
	//heroineTextures.push_back(textureInfo);*/


	////// Body
	//// albedo
	//textureInfo.meshName = "Mesh.025";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Body_BaseColor.tga";
	//textureInfo.textureType = "albedoMap";
	//heroineTextures.push_back(textureInfo);
	//// normal
	//textureInfo.meshName = "Mesh.025";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Body_Normal.tga";
	//textureInfo.textureType = "normalMap";
	//heroineTextures.push_back(textureInfo);
	//// roughness
	//textureInfo.meshName = "Mesh.025";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Body_Roughness.tga";
	//textureInfo.textureType = "roughnessMap";
	//heroineTextures.push_back(textureInfo);
	//// metallic
	//textureInfo.meshName = "Mesh.025";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	//textureInfo.textureType = "metallicMap";
	//heroineTextures.push_back(textureInfo);
	//// opacity
	///*textureInfo.meshName = "Mesh.025";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/white_map.png";
	//textureInfo.textureType = "opacityMap";
	//heroineTextures.push_back(textureInfo);*/

	////// Head
	//// albedo
	//textureInfo.meshName = "Mesh.025.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Head_BaseColor.tga";
	//textureInfo.textureType = "albedoMap";
	//heroineTextures.push_back(textureInfo);
	//// normal
	//textureInfo.meshName = "Mesh.025.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Head_Normal.tga";
	//textureInfo.textureType = "normalMap";
	//heroineTextures.push_back(textureInfo);
	//// roughness
	//textureInfo.meshName = "Mesh.025.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Head_Roughness.tga";
	//textureInfo.textureType = "roughnessMap";
	//heroineTextures.push_back(textureInfo);
	//// metallic
	//textureInfo.meshName = "Mesh.025.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	//textureInfo.textureType = "metallicMap";
	//heroineTextures.push_back(textureInfo);
	//// opacity
	///*textureInfo.meshName = "Mesh.025.1";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/white_map.png";
	//textureInfo.textureType = "opacityMap";
	//heroineTextures.push_back(textureInfo);*/


	////// Unknown
	//// albedo
	//textureInfo.meshName = "Mesh.021";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_BaseColor.tga";
	//textureInfo.textureType = "albedoMap";
	//heroineTextures.push_back(textureInfo);
	//// normal
	//textureInfo.meshName = "Mesh.021";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_Normal.tga";
	//textureInfo.textureType = "normalMap";
	//heroineTextures.push_back(textureInfo);
	//// roughness
	///*textureInfo.meshName = "Mesh.021";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Head_Roughness.tga";
	//textureInfo.textureType = "roughnessMap";
	//heroineTextures.push_back(textureInfo);*/
	//// metallic
	//textureInfo.meshName = "Mesh.021";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	//textureInfo.textureType = "metallicMap";
	//heroineTextures.push_back(textureInfo);
	//// opacity
	//textureInfo.meshName = "Mesh.021";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_Opacity.tga";
	//textureInfo.textureType = "opacityMap";
	//heroineTextures.push_back(textureInfo);


	////// Eyes
	//// albedo
	//textureInfo.meshName = "Mesh.023";
	//textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Eyes_BaseColor.tga";
	//textureInfo.textureType = "albedoMap";
	//heroineTextures.push_back(textureInfo);
	// normal
	/*textureInfo.meshName = "Mesh.021";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Hair_Normal.tga";
	textureInfo.textureType = "normalMap";
	heroineTextures.push_back(textureInfo);*/
	// roughness
	/*textureInfo.meshName = "Mesh.021";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/MHXX_Head_Roughness.tga";
	textureInfo.textureType = "roughnessMap";
	heroineTextures.push_back(textureInfo);*/
	// metallic
	/*textureInfo.meshName = "Mesh.021";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/metallic_map.png";
	textureInfo.textureType = "metallicMap";
	heroineTextures.push_back(textureInfo);*/
	// opacity
	/*textureInfo.meshName = "Mesh.023";
	textureInfo.path = "C:/Users/getra/source/repos/OpenGL Playground/OpenGL Playground/textures/white_map.png";
	textureInfo.textureType = "opacityMap";
	heroineTextures.push_back(textureInfo);*/
}

