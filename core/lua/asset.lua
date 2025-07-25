nux._assets = {}

function nux.mesh.get(url)
    if nux._assets[url] then
        return nux._assets[url]
    else
        nux._assets[url] = nux.mesh.load(url)
    end
end

function nux.scene.get(url)
    if nux._assets[url] then
        return nux._assets[url]
    else
        nux._assets[url] = nux.scene.load(url)
    end
end
