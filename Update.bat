////   ɾ 			//  ɼ 		//              ִ    ġ			//  纻                 ġ

xcopy			/y			.\0.Skul_Resource\*.*				.\Tool\Bin\Resources\Sprite\	
xcopy			/y			.\0.Skul_Resource\*.*				.\Client\Bin\Resources\Sprite\	

xcopy			/y			.\Engine\Bin\Engine.dll			.\Client\Bin\	
xcopy			/y			.\Engine\Bin\Engine.dll			.\Tool\Bin\	
xcopy			/y			.\Engine\Bin\Engine.lib			.\Reference\Librarys\	
xcopy			/y			.\Engine\ThirdPartyLib\*.lib			.\Reference\Librarys\	
xcopy			/y/s		.\Engine\Public\*.*				.\Reference\Headers\
xcopy			/y			.\Engine\Bin\ShaderFiles\*.*	.\Client\Bin\ShaderFiles\	
xcopy			/y			.\Engine\Bin\ShaderFiles\*.*	.\Tool\Bin\ShaderFiles\	
xcopy			/y			.\Tool\Bin\DataFiles\*.*		.\Client\Bin\DataFiles\