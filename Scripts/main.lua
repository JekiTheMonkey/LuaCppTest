FIRE_COUNT          = 120
MIN_VEL             = -5
MAX_VEL             = 5
FIRE_WIDTH          = 64
FIRE_HEIGHT         = 64
HALF_FIRE_WIDTH     = FIRE_WIDTH / 2
HALF_FIRE_HEIGHT    = FIRE_HEIGHT / 2
FIRE_FRAME_COUNT    = 64
FIRE_MAX_FRAME      = FIRE_FRAME_COUNT - 1
ATLAS_WIDTH         = 640
ATLAS_HEIGHT        = 384
ATLAS_HORIZONTAL    = 10
ATLAS_VERTICAL      = 6
WINDOW_WIDTH        = 1024
WINDOW_HEIGHT       = 768

Fires = {}

-- Lua's one doesn't work :(
function fmod(x, y)
    while x >= y do
        x = x - y
    end
    return x
end

function GetRect(Index)
    local rect = {}

    rect.left   = fmod(Index, ATLAS_HORIZONTAL) * FIRE_WIDTH
    rect.top    = fmod(Index, ATLAS_VERTICAL) * FIRE_HEIGHT 
    rect.width  = FIRE_WIDTH
    rect.height = FIRE_HEIGHT

    return rect
end

function Init()
    -- Load the fire atlas, create its sprite
    LoadTexture("Fire", "../resources/fire1_64.png")
    CreateSprite("Fire", "Fire")

    -- Initialize all the fires
    for FireIndex = 1, FIRE_COUNT do
        -- Create a new table to hold all of the fire's fields
        local CurrFire = {}

        -- Set the X, Y location
        CurrFire.X = RandInt(0, WINDOW_WIDTH - FIRE_WIDTH)
        CurrFire.Y = RandInt(0, WINDOW_HEIGHT - FIRE_HEIGHT)

        -- Set the X, Y velocity
        CurrFire.XVel = RandInt(MIN_VEL, MAX_VEL)
        CurrFire.YVel = RandInt(MIN_VEL, MAX_VEL)

        -- Set the current frame and the rect
        CurrFire.Frame = RandInt(0, FIRE_MAX_FRAME)
        CurrFire.Rect = GetRect(CurrFire.Frame)

        -- Get the texture rectangle
        local left =    CurrFire.Rect.left
        local top =     CurrFire.Rect.top
        local width =   CurrFire.Rect.width
        local height =  CurrFire.Rect.height

        -- Set texture rectangle
        SetSpriteRect("Fire", left, top, width, height)

        -- Copy the reference to the new fire into the table
        Fires[FireIndex] = CurrFire
    end
end

function Update()
    for FireIndex = 1, FIRE_COUNT do
        -- Get the X, Y location
        local X = Fires[FireIndex].X;
        local Y = Fires[FireIndex].Y;

        -- Get the texture rectangle
        local left =    Fires[FireIndex].Rect.left
        local top =     Fires[FireIndex].Rect.top
        local width =   Fires[FireIndex].Rect.width
        local height =  Fires[FireIndex].Rect.height

        -- Procede on next frame
        Fires[FireIndex].Frame = fmod(Fires[FireIndex].Frame + 1, FIRE_MAX_FRAME)
        Fires[FireIndex].Rect = GetRect(Fires[FireIndex].Frame)
        SetSpriteRect("Fire", left, top, width, height)
        
        -- Randomize new X, Y velocity
        local XVel = Fires[FireIndex].XVel
        local YVel = Fires[FireIndex].YVel
        
        -- Affect the position by the velocity
        X = X + XVel
        Y = Y + YVel
        
        -- Check for wall collision
        if X > WINDOW_WIDTH or X < 0 then
            XVel = -XVel
        end
        if Y > WINDOW_HEIGHT or Y < 0 then
            YVel = -YVel
        end

        -- Set the X, Y velocity
        Fires[FireIndex].XVel = XVel
        Fires[FireIndex].YVel = YVel

        -- Set the X, Y location
        Fires[FireIndex].X = X
        Fires[FireIndex].Y = Y
    end
end

function Render()
    -- Clear previous drawings
    ClearWindow()

    for FireIndex = 1, FIRE_COUNT do
        -- Get the X, Y location
        local X = Fires[FireIndex].X;
        local Y = Fires[FireIndex].Y;

        -- Draw sprite on X, Y
        DrawSprite("Fire", X, Y)
    end

    -- Display drawn fires
    DisplayOnWindow()
end