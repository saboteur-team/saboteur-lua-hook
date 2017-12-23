#pragma once
namespace gui
{
	//---------------------------------------
	//			 FORWARD DECLARATIONS
	//---------------------------------------
	class Font;

	//---------------------------------------
	//			 GLOBAL VARIABLES
	//---------------------------------------
	LPDIRECT3DDEVICE9	internal_device;
	LPD3DXSPRITE		internal_sprite;
	D3DXVECTOR2			internal_resolution;

	std::vector<std::function<void()>> internal_on_draw;
	//---------------------------------------

	//---------------------------------------
	//			 HELPER METHODS 
	//---------------------------------------
	class Font {
	public:
		Font() :
			m_font_height(15),
			m_font_name("Arial"),
			m_font_color(0xFFFFFFFF),
			m_font_sprite(internal_sprite)
		{
		}

		~Font()
		{
			if (m_font != nullptr) {
				m_font->Release();
			}
		}

		auto get_height() { return m_font_height; }


		void set_height(int font_size) { m_font_height = font_size; }
		void set_name(std::string name) { m_font_name = name; }
		void set_color(unsigned long color) { m_font_color = color; }
		void set_sprite(LPD3DXSPRITE sprite) { m_font_sprite = sprite; }

		void set_pos(int x, int y)
		{
			m_font_transform.left	= x;
			m_font_transform.top	= y;
			m_font_transform.right	= internal_resolution.x;
			m_font_transform.bottom = internal_resolution.y;
		}

		void draw(const char* text)
		{
			if (m_font)
				m_font->DrawTextA(m_font_sprite, text, -1, &m_font_transform, NULL, m_font_color);
		}

		void draw(const wchar_t* text)
		{
			if(m_font) 
				m_font->DrawTextW(m_font_sprite, text, -1, &m_font_transform, NULL, m_font_color);
		}

		void create(void)
		{
			if (FAILED(D3DXCreateFontA(internal_device, m_font_height, 0, FW_NORMAL, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, m_font_name.c_str(), &m_font))) {
				MessageBoxA(NULL, "Unable to create directx font !", "ERROR", MB_OK);
			}
		}
	private:
		LPD3DXFONT		m_font;
		LPD3DXSPRITE	m_font_sprite;
		int				m_font_height;
		bool			m_font_wide;
		std::string		m_font_name;
		RECT			m_font_transform;
		unsigned long	m_font_color;
	};
	//---------------------------------------
	
	struct Vertex_t
	{
		XMFLOAT4 xyzrhw;
		D3DCOLOR color;

		enum
		{
			FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE
		};
	};

	inline void buid_vertex(XMFLOAT4 xyzrhw, D3DCOLOR color, Vertex_t* vertexList, int index)
	{
		vertexList[index].xyzrhw = xyzrhw;
		vertexList[index].color = color;
	}

	void draw_rect(int x, int y, int w, int h, D3DCOLOR color)
	{
		Vertex_t vertex_list[4];
		buid_vertex(XMFLOAT4(x, y + h, 0, 1), color, vertex_list, 0);
		buid_vertex(XMFLOAT4(x, y, 0, 1), color, vertex_list, 1);
		buid_vertex(XMFLOAT4(x + w, y + h, 0, 1), color, vertex_list, 2);
		buid_vertex(XMFLOAT4(x + w, y, 0, 1), color, vertex_list, 3);

		internal_device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
		internal_device->SetFVF(Vertex_t::FVF);
		internal_device->SetTexture(0, NULL);
		internal_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex_list, sizeof(Vertex_t));
	}

	/*
	*	Gets resolution of current window
	*/
	inline D3DXVECTOR2 get_resolution()
	{
		IDirect3DSurface9* back_buffer;
		internal_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);

		D3DSURFACE_DESC surface_description;
		back_buffer->GetDesc(&surface_description);

		D3DXVECTOR2 returnVec = D3DXVECTOR2(surface_description.Width, surface_description.Height);
		back_buffer->Release();

		return returnVec;
	}

	/* 
	* Inits our gui stuff
	*/
	inline void init(IDirect3DDevice9* device)
	{
		if (device != nullptr) {

			internal_device = device;

			if (FAILED(D3DXCreateSprite(device, &internal_sprite))) {
				MessageBoxA(NULL, "Unable to create sprite !", "ERROR", MB_OK);
			}

			internal_resolution = get_resolution();
		}
	}

	/*
	* Procedure wich adds event into our ondraw event array 
	*/
	inline void on_draw(std::function<void()> callback)
	{
		if(callback != nullptr)
			internal_on_draw.push_back(callback);
	}

	/* 
	* Procedure wich is called every frame
	* Renders all our GUI objects
	*/
	inline void render()
	{
		if (internal_sprite != nullptr) {
			internal_sprite->Begin(D3DXSPRITE_ALPHABLEND);

			for (auto event : internal_on_draw) {
				event();
			}

			internal_sprite->End();
		}
	}
};