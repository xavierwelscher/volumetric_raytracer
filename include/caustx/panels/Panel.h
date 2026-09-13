#pragma once

class IPanel {
				public:
								virtual ~IPanel() = default;
								virtual void onRender() = 0;
};
