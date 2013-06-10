#include "main_window.h"
#include "application.h"

/*
Это стартовый модуль, с которого начинается выполнение.
*/

//** оконные процедуры главного окна

//стартовая функция
INT WINAPI wWinMain(HINSTANCE,HINSTANCE,LPTSTR,INT)
{
#ifdef _DEBUG
	//выполнить при завершении программы вывод информации об утечках памяти
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	//инициализировать приложение
	gApplication.Initialize();

	//создать главное окно
	CMainWindow MainWindow;

	//выполнять программу!
	MainWindow.Show();

	//выполнить очистку ресурсов
	gApplication.Uninitialize();

	return 0;
}
