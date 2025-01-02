#include "Header.h"

int main() {
	//GetComPath();
	ComPath = filesystem::current_path().string();
	for (char& c : ComPath) {
		c = (c == '\\') ? '/' : c;
	}
	while (ComPath.back() != '/') {
		ComPath.pop_back();
	}
	ComPath += "Common/";
	UIPath = ComPath + "UI.txt";
	SystemPath = ComPath + "System.txt";
	modifiedTime = filesystem::last_write_time(SystemPath);
	//Main loop
	InitWindow(SCREENWIDTH, SCREENHEIGHT, "Test Window");
	textures.emplace_back(LoadTexture("_Data/Images/user.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/server.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/back.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/app.png"));  
	textures.emplace_back(LoadTexture("_Data/Images/process.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/off.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/reset.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/copy.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/delete.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/screenshot.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/keylogger.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/keyloggerOff.png"));
	textures.emplace_back(LoadTexture("_Data/Images/webcam.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/webcamOff.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/user2.png")); 
	textures.emplace_back(LoadTexture("_Data/Images/pc.png"));

	animations.emplace_back(LoadTexture("_Data/Images/emptySheet.png")); 
	animations.emplace_back(LoadTexture("_Data/Images/waitingAcceptSheet.png")); 
	animations.emplace_back(LoadTexture("_Data/Images/waitingSheet.png"));
	//Waiting for system load succesfully
	while (true) {
		auto newModifiedTime = filesystem::last_write_time(SystemPath);
		if (newModifiedTime != modifiedTime) {
			modifiedTime = newModifiedTime;
			break;
		}
	}
	//Loading animation
	///////////////////////////////////////////////////////////// MAIN LOOP
	while (!WindowShouldClose()) {
		BeginDrawing();

		ClearBackground(background);
		//Vẽ background

		Draw();
		if (isUserInputChanged) {
			fstream file(UIPath, ios::out);
			auto now = std::chrono::system_clock::now();

			// Chuyển đổi thời gian thành time_t để in phần ngày giờ
			std::time_t current_time = std::chrono::system_clock::to_time_t(now);
			auto current_time_micro = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;

			// Định dạng và in thời gian
			std::cout << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S")
				<< '.' << std::setw(6) << std::setfill('0') << current_time_micro.count()
				<< std::endl;
			file << userInput;
			file.close();
			now = std::chrono::system_clock::now();

			// Chuyển đổi thời gian thành time_t để in phần ngày giờ
			current_time = std::chrono::system_clock::to_time_t(now);
			current_time_micro = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()) % 1000000;

			// Định dạng và in thời gian
			std::cout << std::put_time(std::localtime(&current_time), "%Y-%m-%d %H:%M:%S")
				<< '.' << std::setw(6) << std::setfill('0') << current_time_micro.count()
				<< std::endl;
			isUserInputChanged = false;

		}

		EndDrawing();
	}
	CloseWindow();
	return 0;
}
