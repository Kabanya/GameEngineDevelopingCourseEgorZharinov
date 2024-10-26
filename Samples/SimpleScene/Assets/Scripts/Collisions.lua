local ecs = require "ecs"

local TimeRemaining = 0.0

local function UpdateLifeTime(it)
    for LifeTime, ent in ecs.each(it) do
        LifeTime.value = LifeTime.value - it.delta_time
    end
end

local function DestructionSystem(it)
    for pos, LifeTime, ent in ecs.each(it) do
        if LifeTime.value <= TimeRemaining then
            pos.y = pos.y + 10000
        end
    end
end

-- 1st version
-- local function DetectCollisions(it)
    -- for pos1, size1 in ecs.each(it) do
        -- it.pos1 = pos1
        -- it.size1 = size1
    -- end
-- end

-- local function ApplyCollisions(it)
    -- for pos2, vel2, size2 in ecs.each(it) do
        -- if it.pos1 and pos2 and it.size1 and size2 and CheckCollision(it.pos1, pos2, it.size1, size2) then
            -- local m1 = it.size.x * it.size.y * it.size.z
            -- local m2 = size2.x * size2.y * size2.z
        -- end
    -- end
-- end

--2nd version
local function DetectCollisions(it)
	for pos, size, ent in ecs.each(it) do
		for pos2, size2, ent2 in ecs.each(it) do
			if ent ~= ent2 then
				local overlap_x = math.abs(pos.x - pos2.x) < (size.x + size2.x) / 2
				local overlap_y = math.abs(pos.y - pos2.y) < (size.y + size2.y) / 2
				local overlap_z = math.abs(pos.z - pos2.z) < (size.z + size2.z) / 2
				if overlap_x and overlap_y and overlap_z then
					coll.value = true
				end
			end
		end
	end
end

local function ApplyCollisions(it)
    for vel, coll, ent in ecs.each(it) do
        if coll.value then
			vel.y = coll.value * 2.5
		end
    end
end



ecs.system(UpdateLifeTime, "UpdateLifeTime", ecs.OnUpdate, "LifeTime")
ecs.system(DestructionSystem, "DestructionSystem", ecs.OnUpdate, "Position, LifeTime")

ecs.system(DetectCollisions, "DetectCollisions", ecs.OnUpdate, "Position, ColliderIs")
ecs.system(ApplyCollisions, "ApplyCollisions", ecs.OnUpdate, "Velocity, ColliderIs, JumpOnCollision")