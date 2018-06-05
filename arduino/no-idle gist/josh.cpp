std::pair<int, int> noIdleZones[] = 
  {{1, 42}, {80, 111}, {145, 155}, {295, 305}, {445, 455}, {595, 605}, {718, 749}};

int CoerceTargetPixel(int targetPixel) {
  for (auto zone : noIdleZones)
    if (targetPixel >= zone.first && targetPixel <= zone.second) {
      ReturnClosestPixel(targetPixel, zone.first, zone.second);
      return targetPixel;
    }
  }
   return targetPixel;
}