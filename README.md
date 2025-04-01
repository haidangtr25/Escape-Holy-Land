**Đồ án Game: Escape Holy Land**

**Thông tin sinh viên:**
Tên: Trần Doãn Hải Đăng  
Mã sinh viên: 24020057  

**Giới thiệu game:**  
Escape Holy Land là trò chơi hành động phiêu lưu nơi người chơi điều khiển nhân vật vượt qua các chướng ngài vật thánh để thoát khỏi vùng đất linh thiêng. Game có đồ họa 2D, hệ thống âm thanh đa dạng và nhiều chức năng nâng cao.  

**Ngưỡng điểm mục tiêu: 9-10**  

**Lý do:**  

1. **Tính năng tự làm và tự học:**  
   - Hệ thống vật lý: Triển khai hệ thống trọng lực, va chạm.  
   - Đồ họa 2D: Sử dụng SDL2 để render hình ảnh, texture, hoạt hình.  
   - Âm thanh: Nhạc nền và hiệu ứng âm thanh theo sự kiện.  
   - Hệ thống power-up: 3 loại vật phẩm tăng sức mạnh với hiệu ứng riêng.  
   - Hệ thống đối tượng: Nhiều loại chướng ngài vật với hành vi khác nhau.  
   - Menu hệ thống: Giao diện chính, chọn mức độ, nút Pause.  
   - Lưu điểm cao: Hệ thống đọc/ghi file lưu điểm cao nhất.  
   - Độ khó tăng dần: Độ khó thay đổi theo điểm số đạt được.  

2. **Thuật toán đã cài đặt:**  
   - Thuật toán phát hiện va chạm.  
   - Thuật toán di chuyển theo quã đạo cho vật thể.  
   - Thuật toán bắn vật thể theo nhiều hướng.  
   - Thuật toán theo dõi người chơi cho boss.  
   - Thuật toán quản lý thời gian hiệu lực power-up.  

3. **Điểm nổi bật:**  
   - Đa dạng chướng ngài vật: 5 loại với hành vi khác nhau.  
   - Hệ thống level: 4 mức độ khó (Easy, Medium, Hard, Hell).  
   - Hiệu ứng hình ảnh: Xoay vật thể, thay đổi hướng theo di chuyển.  
   - Giao diện trực quan: Hiển thị trạng thái, điểm số, nút Pause rõ ràng.  
   - Hệ thống âm thanh: Nhạc nền và hiệu ứng phù hợp với từng sự kiện.  

**Hướng dẫn cài đặt và chạy game:**  
1. Cài đặt thư viện SDL2, SDL_image, SDL_ttf, SDL_mixer.  
2. Clone repository.  
3. Biên dịch và chạy.  

**Nguồn tham khảo:**  
- **Âm thanh:**  
  - Background: https://www.youtube.com/watch?v=buu6yHDGVU4  
  - Sounds xuất hiện cùng objects: https://pixabay.com/sound-effects/search/game/  
- **Đồ họa:**  
  - Tìm trên Google các file PNG phù hợp, remove background: https://www.remove.bg/vi/upload  
  - Crop ảnh: https://www.iloveimg.com/  
  - Chuyển đổi file: Website hỗ trợ chức năng  
  - Hỗ trợ AI (ChatGPT / Grok) tạo hình ảnh pixel  

**Sử dụng AI hỗ trợ:**  
- ChatGPT: Hỗ trợ debug code, gợi ý giải pháp kỹ thuật.  
- GitHub Copilot: Hỗ trợ auto-complete code.  
- Mức độ sử dụng: Khoảng 40% code có sự hỗ trợ từ AI.  
