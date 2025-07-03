nux.ASSETS = {}

function nux.mesh.get(url)
    if nux.ASSETS[url] then
        return nux.ASSETS[url]
    else
        nux.ASSETS[url] = nux.mesh.load(url)
    end
end
